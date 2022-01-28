/*
 *   Copyright (C) 2014,2018,2020 by Jonathan Naylor G4KLX
 *   APRSTransmit Copyright (C) 2015 Geoffrey Merck F4FXL / KC3FRA
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "DStarDefines.h"
#include "APRSTransmit.h"
#include "APRSTransmitAppD.h"

#include <wx/textfile.h>
#include <wx/cmdline.h>
#include <signal.h>
#include <iostream>

#if defined(__WINDOWS__)
#include <Windows.h>
#include <wx/filename.h>
#endif

const wxChar* REPEATER_PARAM   = wxT("Repeater");
const wxChar* APRS_HOST        = wxT("host");
const wxChar* APRS_PORT        = wxT("port");
const wxChar* DAEMON_SWITCH    = wxT("daemon");

static CAPRSTransmitAppD* m_aprsTransmit = NULL;

static void handler(int signum)
{
	m_aprsTransmit->kill();
}

int main(int argc, char** argv)
{
	bool res = ::wxInitialize();
	if (!res) {
		::fprintf(stderr, "aprstransmit: failed to initialise the wxWidgets library, exiting\n");
		return 1;
	}

	wxCmdLineParser parser(argc, argv);
	parser.AddParam(REPEATER_PARAM, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddOption(APRS_HOST, wxEmptyString, wxEmptyString, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddOption(APRS_PORT, wxEmptyString, wxEmptyString, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
	parser.AddSwitch(DAEMON_SWITCH, wxEmptyString, wxEmptyString, wxCMD_LINE_PARAM_OPTIONAL);

	int cmd = parser.Parse();
	if (cmd != 0) {
		::wxUninitialize();
		return 1;
	}

	if (parser.GetParamCount() < 1U) {
		::fprintf(stderr, "aprstransmitd: invalid command line usage: aprstransmitd <repeater> [-host <aprs_server>] [-port <aprs_port>] [-daemon] exiting\n");
		::wxUninitialize();
		return 1;
	}

	wxString repeater = parser.GetParam(0U);
	if (repeater.length() != LONG_CALLSIGN_LENGTH) {
		wxString callErrorMsg;
		callErrorMsg << wxT("Invalid repeater call. ") << repeater << wxT("Call must be ") << LONG_CALLSIGN_LENGTH << wxT(" characters long.\n");
		callErrorMsg << wxT("Valid example : A1ABC__B\n");
		::fputs(callErrorMsg.c_str(), stderr);
		::wxUninitialize();
		return 1;
	}
	repeater.Replace(wxT("_"), wxT(" "));
	repeater.MakeUpper();

	long aprsPort;
	if (!parser.Found(APRS_PORT, &aprsPort))
		aprsPort = 14580L;

	wxString aprsHost;
	if (!parser.Found(APRS_HOST, &aprsHost))
		aprsHost = wxT("rotate.aprs2.net");

	bool daemon = parser.Found(DAEMON_SWITCH);

#if defined(__WINDOWS__)
	m_aprsTransmit = new CAPRSTransmitAppD(repeater, aprsHost, aprsPort, daemon);
	if (!m_aprsTransmit->init()) {
		::wxUninitialize();
		return 1;
	}
#else
	if (daemon) {
		pid_t pid = ::fork();

		if (pid < 0) {
			::fprintf(stderr, "aprstransmitd: error in fork(), exiting\n");
			::wxUninitialize();
			return 1;
		}

		// If this is the parent, exit
		if (pid > 0)
			return 0;

		// We are the child from here onwards
		::setsid();
		::chdir("/");
		::umask(0);
	}

	//create a pid file
	wxString pidFileName = wxT("/var/run/aprstransmit.pid");
	FILE* fp = ::fopen(pidFileName.mb_str(), "wt");
	if (fp != NULL) {
		::fprintf(fp, "%u\n", ::getpid());
		::fclose(fp);
	}

	m_aprsTransmit = new CAPRSTransmitAppD(repeater, aprsHost, aprsPort, daemon);
	if (!m_aprsTransmit->init()) {
		::wxUninitialize();
		return 1;
	}

	::signal(SIGUSR1, handler);


	::unlink(pidFileName.mb_str());
#endif
	m_aprsTransmit->run();
	delete m_aprsTransmit;
	::wxUninitialize();
	return 0;
}



CAPRSTransmitAppD::CAPRSTransmitAppD(const wxString& repeater, const wxString& aprsHost, unsigned int aprsPort, bool daemon) :
m_aprsFramesQueue(NULL),
m_repeater(repeater),
m_aprsHost(aprsHost),
m_aprsPort(aprsPort),
m_aprsSocket(NULL),
m_run(false),
m_checker(NULL),
m_daemon(daemon)
{
}

CAPRSTransmitAppD::~CAPRSTransmitAppD()
{
	cleanup();
}


bool CAPRSTransmitAppD::init()
{
#if defined(__WINDOWS__)
	wxString tempPath = wxFileName::GetTempDir();
	m_checker = new wxSingleInstanceChecker(wxT("aprstransmit"), tempPath);
#else
	m_checker = new wxSingleInstanceChecker(wxT("aprstransmit"), wxT("/tmp"));
#endif
	bool ret = m_checker->IsAnotherRunning();
	if (ret) {
		wxLogError(wxT("Another copy of APRSTransmit is running, exiting"));
		return false;
	}

#if defined(__WINDOWS__)
	wxLog* logger = new wxLogStream(&std::cout);
	wxLog::SetActiveTarget(logger);
	wxLog::SetLogLevel(wxLOG_Message);
	if(m_daemon)
		wxLogMessage(wxT("Daemon not supported under Windows, ignoring"));
	m_daemon = false;
#else
	if(!m_daemon){
		wxLog* logger = new wxLogStream(&std::cout);
		wxLog::SetActiveTarget(logger);
		wxLog::SetLogLevel(wxLOG_Message);
	} else {
		new wxLogNull;
	}
#endif

	return true;
}

void CAPRSTransmitAppD::run()
{
	if(m_run) return;

	m_aprsFramesQueue = new CRingBuffer<wxString*>(30U);
	m_aprsSocket = new CUDPReaderWriter;
	m_aprsSocket->open();
	
	wxString * aprsFrame;

	m_run = true;
	while(m_run){
		wxMilliSleep(10U);
		aprsFrame = m_aprsFramesQueue->getData();
		if (aprsFrame != NULL) {
			CAPRSTransmit aprsTransmit(m_repeater, wxString(*aprsFrame));
			aprsTransmit.run();
			delete aprsFrame;
		}
	}

	cleanup();
}


void CAPRSTransmitAppD::cleanup()
{
	if (m_aprsFramesQueue != NULL)
	{
		while (m_aprsFramesQueue->peek()) delete m_aprsFramesQueue->getData();
		delete m_aprsFramesQueue;
		m_aprsFramesQueue = NULL;
	}	

	if (m_checker != NULL) {
		delete m_checker;
		m_checker = NULL;
	}

	if (m_aprsSocket != NULL)
	{
		m_aprsSocket->close();
		delete m_aprsSocket;
		m_aprsSocket = NULL;
	}
}

void CAPRSTransmitAppD::kill()
{
	m_run = false;
}

