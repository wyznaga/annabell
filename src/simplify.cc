/*
 Copyright (C) 2015 Bruno Golosio

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Annabell.h>
#include <Monitor.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "interface.h"
#include "display.h" 

using namespace std;

int ParseCommand(Annabell *annabell, Monitor *monitor, string input_line);

bool simplify(Annabell *annabell, Monitor *monitor, vector<string> input_token) {

	string cmd, buf; // buffer string
	string cmd_line; //command_line
	bool push_flag;

	if (input_token.size() <= 1) {
		return false;
	}

	cmd = input_token[0];

	if (cmd == ".wg*") {
		cmd = ".wg";
		push_flag = true;
	} else if (cmd == ".po*") {
		cmd = ".po";
		push_flag = true;
	} else if (cmd == ".o*") {
		cmd = ".o";
		push_flag = true;
	} else {
		push_flag = false;
	}

	if (input_token.size() >= 2 && (cmd == ".wg" || cmd == ".o" || cmd == ".po" || cmd == ".pg" || cmd == ".ph")) {
		buf = input_token[1];

		if (input_token.size() == 2 && buf[0] == '/') {
			unsigned int i;
			string s1, s1b, s2, ph;
			i = 0;
			s1 = s1b = s2 = ph = "";

			for (i = 1; i < buf.size(); i++) {
				if (buf[i] == '/' || buf[i] == '&') {
					break;
				}

				s1 = s1 + buf[i];
			}

			if (i >= buf.size()) {
				Display.Warning("Syntax error 1 on simplifying");
				return true;
			}

			if (buf[i] == '&') {
				for (i++; i < buf.size(); i++) {
					if (buf[i] == '/') {
						break;
					}

					s1b = s1b + buf[i];
				}
				if (i == buf.size()) {
					Display.Warning("Syntax error 2 on simplifying");
					return true;
				}
			}

			for (i++; i < buf.size(); i++) {
				if (buf[i] == '/') {
					break;
				}

				ph = ph + buf[i];
			}

			if (i == buf.size()) {
				Display.Warning("Syntax error 3 on simplifying");
				return true;
			}

			for (i++; i < buf.size(); i++) {
				if (buf[i] == '/') {
					break;
				}

				s2 = s2 + buf[i];
			}

			if (i == buf.size()) {
				Display.Warning("Syntax error 4 on simplifying");
				return true;
			}

			if (ph == "" && s2 == "") {
				Display.Warning("Syntax error 5 on simplifying");
				return true;
			}

			// if working phrase must be stored and retrieved at the end
			if (push_flag && ParseCommand(annabell, monitor, ".push_goal") == 2) {
				return true;
			}

			if (push_flag && ParseCommand(annabell, monitor, ".ggp") == 2) {
				return true;
			}

			if (s1b != "") { // if there is a second cue

				if (ParseCommand(annabell, monitor, string(".wg ") + s1b) == 2) {
					return true;
				}

				if (ParseCommand(annabell, monitor, ".push_goal") == 2) {
					return true;
				}

				if (ParseCommand(annabell, monitor, ".ggp") == 2) {
					return true;
				}
			}

			if (s1 != "") {
				if (ParseCommand(annabell, monitor, string(".wg ") + s1) == 2) {
					return true;
				}
			} else {
				monitor->GetPhM("WGB", annabell->WGB);
				s1 = monitor->OutPhrase;
			}

			cmd_line = ".ph ";

			if (ph == "") {
				cmd_line = cmd_line + s1 + " , " + s2; // standard substitution
			}
			else {
				cmd_line = cmd_line + ph;
			}

			if (ParseCommand(annabell, monitor, cmd_line) == 2) {
				return true;
			}

			if (s2 != "" && ParseCommand(annabell, monitor, string(".wg ") + s2) == 2) {
				return true;
			}

			if (s1b != "") { // drop goal if there was a second cue
				if (ParseCommand(annabell, monitor, ".drop_goal") == 2) {
					return true;
				}
			}

			// retrieve working phrase if necessary
			if (push_flag && ParseCommand(annabell, monitor, ".ggp") == 2) {
				return true;
			}
			if (push_flag && ParseCommand(annabell, monitor, ".drop_goal") == 2) {
				return true;
			}

		} else if (cmd == ".ph" || cmd == ".wg") {
			return false;
		}
		else {

			if (push_flag && ParseCommand(annabell, monitor, ".push_goal") == 2) {
				return true;
			}
			if (push_flag && ParseCommand(annabell, monitor, ".ggp") == 2) {
				return true;
			}

			cmd_line = ".wg";

			for (unsigned int i = 1; i < input_token.size(); i++) {
				buf = input_token[i];
				cmd_line = cmd_line + " " + buf;
			}

			if (ParseCommand(annabell, monitor, cmd_line) == 2) {
				return true;
			}

			if (push_flag && ParseCommand(annabell, monitor, ".drop_goal") == 2) {
				return true;
			}
		}

		if (cmd == ".po") {
			ParseCommand(annabell, monitor, ".prw");
		}
		else if (cmd == ".o") {
			ParseCommand(annabell, monitor, ".rw");
		}
		else if (cmd == ".pg") {
			ParseCommand(annabell, monitor, ".push_goal");
		}

		return true;
	}

	return false;
}
