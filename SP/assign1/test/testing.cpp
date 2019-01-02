/*
 * =====================================================================================
 *
 *       Filename:  testing.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/2018 09:38:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:  
 *
 * =====================================================================================
 */
// CPP program to convert hexadecimal to
// // integer using stringstream and
// // hex I/O manipulator.
#include <iostream>
#include <bits/stdc++.h>
 using namespace std;
//  
  int main()
  {
    stringstream ss;
    ss << hex << 99999999999999999999999999999999999999999999999998972348298472987942;
    string res = ss.str();
    cout << "0x" << res << endl;

      return 0;
  }
                              

