#pragma once

#include "User.h"
#include "stdc++.h"

using namespace std;

class Session
{
public:
	User s_User;
	int testNo;
	int autoFlag;
	int login;
	stringstream isstream, osstream;
	Session() {
		autoFlag = 0;
		testNo = 0;
		login = 0;
	}
	~Session() {

	}
};