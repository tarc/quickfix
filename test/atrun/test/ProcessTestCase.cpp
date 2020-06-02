/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#include <gtest/gtest.h>
#include <fstream>
#include "../Process.h"

using namespace ATRUN;

std::string chompDriveLetter(const std::string& s)
{
  std::string::size_type pos = s.find_first_of('\\');
  if(pos == std::string::npos)
    return s;

  std::string result = s;
  result.replace(0,pos+1,"\\");

  return result;
}

static std::string g_startDir("\\projects\\quickfix\\test");

TEST(ProcessTests, createProcess)
{
  SetCurrentDirectory( const_cast<char*>(g_startDir.c_str()) );
  Process object;

  object.setCommandLine("noproc yada yada");
  object.setWorkingDirectory("..");

  object.create();
  ASSERT_TRUE(!object.created());

  object.setCommandLine("cmd /C ECHO Hello World!");

  object.create();
  ASSERT_TRUE(object.created());
}

TEST(ProcessTests, sendOutputToFile)
{
  SetCurrentDirectory( const_cast<char*>(g_startDir.c_str()) );
  Process object;

  std::string expected = "Heidy Ho!";
  std::string cmdl = "cmd /C ECHO ";
  object.setCommandLine(cmdl + expected);

  std::string outfile = "sendOutputToFile.txt";
  object.setOutFile(outfile);
  object.sendToFile(true);

  object.create();
  object.waitFor();

  std::string line;
  std::ifstream f(outfile.c_str());
  std::getline(f,line);
  ASSERT_EQ(expected, line);
}

TEST(ProcessTests, setWorkingDirectory)
{
  SetCurrentDirectory( const_cast<char*>(g_startDir.c_str()) );
  Process object;

  std::string outfile("setWorkingDirectory.txt");

  object.setCommandLine("cmd /C ECHO %CD%");  //write the current directory to file
  object.setOutFile(outfile);
  object.sendToFile(true);
  object.setWorkingDirectory("..");

  object.create();
  object.waitFor();

  std::string line;
  std::ifstream f(outfile.c_str());
  std::getline(f,line);
  ASSERT_EQ("\\projects\\quickfix", chompDriveLetter(line));

  char curdir[100];
  GetCurrentDirectory(sizeof(curdir), curdir);
  std::string expected(curdir);
  ASSERT_EQ( g_startDir, chompDriveLetter(expected) );
}

TEST(ProcessTests, terminateProcess)
{
  SetCurrentDirectory( const_cast<char*>(g_startDir.c_str()) );
  Process object;

  std::string cmdl = "cmd /C PAUSE";
  object.setCommandLine(cmdl);

  std::string outfile("destroyProcess.txt");
  object.setOutFile(outfile);
  object.sendToFile(true);

  object.create();
  ASSERT_TRUE(object.created());
  ASSERT_TRUE(object.getProcessId() > 0);

  object.terminate();
  ASSERT_TRUE(!object.created());
  ASSERT_EQ(-1, object.getProcessId());

  Sleep(1000); //give file a chance to close
  std::ofstream f(outfile.c_str());
  ASSERT_TRUE(!f.fail());
}

TEST(ProcessTests, waitFor)
{
  SetCurrentDirectory( const_cast<char*>(g_startDir.c_str()) );
  Process object;

  std::string outfile("waitfor.txt");

  object.setCommandLine("cmd /C sleep 5");  //requires cygwin
  object.setOutFile(outfile);
  object.sendToFile(true);

  object.create();
  object.waitFor();
  ASSERT_TRUE(!object.created());

  std::ofstream f(outfile.c_str());
  ASSERT_TRUE( !(f.fail()) );

  ASSERT_EQ(-1, object.getProcessId());

  //could also assert that the waitFor() call holds for 5 seconds
  //because of the sleep call
}
