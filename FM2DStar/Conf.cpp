/*
 *   Copyright (C) 2015-2020 by Jonathan Naylor G4KLX
 *   Copyright (C) 2020 by Geoffrey Merck F4FXL - KC3FRA
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

// #include "DStarDefines.h"
#include "Conf.h"
#include "Log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#define MAKE_UPPER(s) for(unsigned int i = 0U; s[i] != 0; i++) s[i] = ::toupper(s[i]);

const int BUFFER_SIZE = 500;

enum SECTION {
	SECTION_NONE,
	SECTION_GENERAL,
	SECTION_LOG,
	SECTION_NETWORK,
	SECTION_DUMMYREPEATER,
	SECTION_DONGLE
};

CConf::CConf(const std::string& file) :
m_file(file),
// General Section
m_callsign(),
m_suffix(),
m_daemon(false),
// Log Section
m_logDisplayLevel(0U),
m_logFileLevel(0U),
m_logFilePath("."),
m_logFileRoot("FM2DStar"),
// Network Section
m_hostAdress("127.0.0.1"),
m_hostPort(3810U),
m_localAddress("127.0.0.1"),
m_localPort(4810U),
m_dummyRptrCallsign(),
m_dummyRptrBand("B"),
//dongle section
m_dongleType(DT_DVDONGLE),
m_dongleAddress("127.0.0.1"),
m_donglePort(2460U),
m_dongleSerialPort("/dev/ttyUSB0")
{
}

CConf::~CConf()
{
}

bool CConf::read()
{
	FILE* fp = ::fopen(m_file.c_str(), "rt");
	if (fp == NULL) {
		::fprintf(stderr, "Couldn't open the .ini file - %s\n", m_file.c_str());
		return false;
	}

	SECTION section = SECTION_NONE;

	char buffer[BUFFER_SIZE];
	while (::fgets(buffer, BUFFER_SIZE, fp) != NULL) {
		if (buffer[0U] == '#')
			continue;

		if (buffer[0U] == '[') {
			if (::strncmp(buffer, "[General]", 9U) == 0)
				section = SECTION_GENERAL;
			else if (::strncmp(buffer, "[Log]", 5U) == 0)
				section = SECTION_LOG;
			else if (::strncmp(buffer, "[Network]", 9U) == 0)
				section = SECTION_NETWORK;
			else if (::strncmp(buffer, "[DummyRepeater]", 15U) == 0)
				section = SECTION_DUMMYREPEATER;
			else if (::strncmp(buffer, "[Dongle]", 8U) == 0)
				section = SECTION_DONGLE;
			else
				section = SECTION_NONE;

			continue;
		}

		char* key   = ::strtok(buffer, " \t=\r\n");
		if (key == NULL)
			continue;

		char* value = ::strtok(NULL, "\r\n");
		if (value == NULL)
			continue;

		// Remove quotes from the value
		size_t len = ::strlen(value);
		if (len > 1U && *value == '"' && value[len - 1U] == '"') {
			value[len - 1U] = '\0';
			value++;
		}

		if (section == SECTION_GENERAL) {
			if (::strcmp(key, "Callsign") == 0) {
				MAKE_UPPER(value);
				m_callsign = m_dummyRptrCallsign = value;
			} else if(::strcmp(key, "Suffix") == 0) {
				MAKE_UPPER(value);
				m_suffix = value;
			} else if(::strcmp(key, "Daemon") == 0)
				m_daemon = !!::atoi(value);
		} else if (section == SECTION_LOG) {
			if (::strcmp(key, "FilePath") == 0)
				m_logFilePath = value;
			else if (::strcmp(key, "FileRoot") == 0)
				m_logFileRoot = value;
			else if (::strcmp(key, "FileLevel") == 0)
				m_logFileLevel = (unsigned int)::atoi(value);
			else if (::strcmp(key, "DisplayLevel") == 0)
				m_logDisplayLevel = (unsigned int)::atoi(value);
		} else if (section == SECTION_NETWORK) {
			if (::strcmp(key, "MMDVMHostAddress") == 0)
				m_hostAdress = value;
			else if (::strcmp(key, "MMDVMHostPort") == 0)
				m_hostPort = ::atoi(value);
			else if (::strcmp(key, "LocalAddress") == 0)
				m_localAddress = value;
			else if (::strcmp(key, "LocalPort") == 0)
				m_localPort = ::atoi(value);
		} else if (section == SECTION_DUMMYREPEATER) {
			if (::strcmp(key, "Callsign") == 0)
				m_dummyRptrCallsign = value;
			else if (::strcmp(key, "Band") == 0 && ::strlen(value) > 1)
				m_dummyRptrBand = value[0];
		} else if (section == SECTION_DONGLE) {
			if (::strcmp(key, "DongleType") == 0) {
				switch (::atoi(value))
				{
				case 1:
					m_dongleType = DT_DVDONGLE;
					break;
				case 2:
					m_dongleType = DT_DV3000_NETWORK;
					break;
				case 3:
					m_dongleType = DT_DV3000_SERIAL;
					break;
				case 4:
					m_dongleType = DT_STARDV_NETWORK;
					break;
				case 5:
					m_dongleType = DT_STARDV_NETWORK2;
					break;
				case 6:
					m_dongleType = DT_DVMEGA_AMBE;
					break;
				default:
					m_dongleType = DT_DVDONGLE;
					break;
				}
			} else if (::strcmp(key, "Address") == 0)
				m_dongleAddress = value;
			else if (::strcmp(key, "Port") == 0)
				m_donglePort = ::atoi(value);
			else if (::strcmp(key, "SerialPort") == 0)
				m_dongleSerialPort = value;
		}
	}
	::fclose(fp);

	return true;
}

// General Section
std::string  CConf::getCallsign() const
{
	return m_callsign;
}

std::string  CConf::getSuffix() const
{
	return m_suffix;
}

bool CConf::getDaemon() const
{
	return m_daemon;
}

// Log Section
unsigned int CConf::getLogDisplayLevel() const
{
	return m_logDisplayLevel;
}

unsigned int CConf::getLogFileLevel() const
{
	return m_logFileLevel;
}

std::string CConf::getLogFilePath() const
{
	return m_logFilePath;
}

std::string CConf::getLogFileRoot() const
{
	return m_logFileRoot;
}

// Network section
std::string CConf::getHostAddress() const
{
	return m_hostAdress;
}
unsigned int CConf::getHostPort() const
{
	return m_hostPort;
}
std::string CConf::getLocalAddress() const
{
	return m_localAddress;
}
unsigned int CConf::getLocalPort() const
{
	return m_localPort;
}

// Dummy Repeater
std::string CConf::getDummyRepeaterCallsign() const
{
	return m_dummyRptrCallsign;
}

std::string CConf::getDummyRepeaterBand() const
{
	return m_dummyRptrBand;
}

// Dongle
DONGLE_TYPE CConf::getDongleType() const
{
	return m_dongleType;
}

std::string CConf::getDongleAddress() const
{
	return m_dongleAddress;
}

unsigned int CConf::getDonglePort() const
{
	return m_donglePort;
}

std::string CConf::getDongleSerialPort() const
{
	return m_dongleSerialPort;
}