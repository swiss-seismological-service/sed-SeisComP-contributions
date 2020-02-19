/***************************************************************************
 *   Copyright (C) by gempa GmbH                                           *
 *                                                                         *
 *   You can redistribute and/or modify this program under the             *
 *   terms of the SeisComP Public License.                                 *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   SeisComP Public License for more details.                             *
 ***************************************************************************/

#define SEISCOMP_TEST_MODULE TestStrings

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <vector>

#include <seiscomp3/unittest/unittests.h>

#include <seiscomp3/core/strings.h>


using namespace std;
using namespace Seiscomp::Core;



vector<string> vec(const char *t1) {
	vector<string> v;
	v.push_back(t1);
	return v;
}

vector<string> vec(const char *t1, const char *t2) {
	vector<string> v;
	v.push_back(t1);
	v.push_back(t2);
	return v;
}

vector<string> vec(const char *t1, const char *t2, const char *t3) {
	vector<string> v;
	v.push_back(t1);
	v.push_back(t2);
	v.push_back(t3);
	return v;
}

vector<string> vec(const char *t1, const char *t2, const char *t3, const char *t4) {
	vector<string> v;
	v.push_back(t1);
	v.push_back(t2);
	v.push_back(t3);
	v.push_back(t4);
	return v;
}


struct TestCase {
	TestCase(const char *delimiter, bool compressOn, const char *source,
	         const vector<string> &tokens, bool trim = false)
	: delimiter(delimiter), compressOn(compressOn), source(source)
	, tokens(tokens), trim(trim) {}

	const char     *delimiter;
	bool            compressOn;
	const char     *source;
	vector<string>  tokens;
	bool            trim;
};

typedef vector<TestCase> TestData;


BOOST_AUTO_TEST_CASE(stringify1) {
	for ( int i = 1; i <= 100; ++i ) {
		string tmp;
		tmp.resize(i);
		for ( size_t j = 0; j < tmp.size(); ++j )
			tmp[j] = random() * 25 / RAND_MAX + 'A';
		string out = stringify("%s", tmp.c_str());
		BOOST_CHECK_EQUAL(tmp, out);
	}
}

BOOST_AUTO_TEST_CASE(split1) {
	TestData data;
	data.push_back(TestCase(",",  true,  ",foo,,,bar,", vec("", "foo", "bar", "")));
	data.push_back(TestCase(",",  true,  ",,foo,bar,,", vec("", "foo", "bar", "")));
	data.push_back(TestCase(" ",  true,  "foo  bar,",   vec("foo", "bar,")));
	data.push_back(TestCase(" ",  false, "foo  bar ",   vec("foo", "", "bar", "")));
	data.push_back(TestCase("oo", false, "fooobar",     vec("f", "", "", "bar")));
	data.push_back(TestCase("oo", true,  "fooooobar",   vec("f", "bar")));
	data.push_back(TestCase("ab", true,  "1a2b3",       vec("1", "2", "3")));

	vector<string> result;
	for ( TestData::const_iterator it = data.begin(); it != data.end(); ++it ) {
		split(result, it->source, it->delimiter, it->compressOn);
		BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
		                              it->tokens.begin(), it->tokens.end());
	}
}

BOOST_AUTO_TEST_CASE(splitExtNoUnescape) {
	typedef vector<TestCase> TestData;

	TestData data;
	data.push_back(TestCase(",",  true,  ",foo,,,bar,",               vec("", "foo", "bar", ""),          false));
	data.push_back(TestCase(",",  true,  ",,foo,bar,,",               vec("", "foo", "bar", ""),          false));
	data.push_back(TestCase(" ",  true,  "foo  bar,",                 vec("foo", "bar,"),                 false));
	data.push_back(TestCase(" ",  false, "foo  bar ",                 vec("foo", "", "bar", ""),          false));
	data.push_back(TestCase("oo", false, "fooobar",                   vec("f", "", "", "bar"),            false));
	data.push_back(TestCase("oo", true,  "fooooobar",                 vec("f", "bar"),                    false));
	data.push_back(TestCase("ab", true,  "1a2b3",                     vec("1", "2", "3"),                 false));
	data.push_back(TestCase(",",  true,  " foo , bar \r\n",           vec("foo", "bar" ),                 true));
	data.push_back(TestCase(",",  true,  " foo \\, bar, , ",          vec("foo \\, bar" ),                true));
	data.push_back(TestCase(",",  true,  " ' foo , bar ', \",bar \"", vec("' foo , bar '", "\",bar \"" ), true));
	data.push_back(TestCase(",",  true,  " \" \\\"foo\" , bar",       vec("\" \\\"foo\"", "bar" ),        true));
	data.push_back(TestCase(",",  true,  " '' \"'\" , \",',' \" ' '", vec("'' \"'\"", "\",',' \" ' '" ),  true));
	data.push_back(TestCase(" ",  true,  "foo bar",                   vec("foo", "bar" ),                 true));
	data.push_back(TestCase(" ",  true,  " foo bar ",                 vec("", "foo", "bar", "" ),         true));
	data.push_back(TestCase(" ",  true,  " foo\\ bar\r\n     a ",     vec("", "foo\\ bar\r\n", "a", "" ), false));

	vector<string> result;
	for ( TestData::const_iterator it = data.begin(); it != data.end(); ++it ) {
		splitExt(result, it->source, it->delimiter, it->compressOn, false, it->trim);
		BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
		                              it->tokens.begin(), it->tokens.end());
	}
}

BOOST_AUTO_TEST_CASE(splitExtUnescape) {
	typedef vector<TestCase> TestData;

	TestData data;
	data.push_back(TestCase(",",  true,  ",foo,,,bar,",               vec("", "foo", "bar", ""),        false));
	data.push_back(TestCase(",",  true,  ",,foo,bar,,",               vec("", "foo", "bar", ""),        false));
	data.push_back(TestCase(" ",  true,  "foo  bar,",                 vec("foo", "bar,"),               false));
	data.push_back(TestCase(" ",  false, "foo  bar ",                 vec("foo", "", "bar", ""),        false));
	data.push_back(TestCase("oo", false, "fooobar",                   vec("f", "", "", "bar"),          false));
	data.push_back(TestCase("oo", true,  "fooooobar",                 vec("f", "bar"),                  false));
	data.push_back(TestCase("ab", true,  "1a2b3",                     vec("1", "2", "3"),               false));
	data.push_back(TestCase(",",  true,  " foo , bar \r\n",           vec("foo", "bar"),                true));
	data.push_back(TestCase(",",  true,  " foo \\, bar, , ",          vec("foo , bar"),                 true));
	data.push_back(TestCase(",",  true,  " ' foo , bar ', \",bar \"", vec(" foo , bar ", ",bar "),      true));
	data.push_back(TestCase(",",  true,  " \" \\t \\\"foo\" , bar",   vec(" \\t \"foo", "bar"),         true));
	data.push_back(TestCase(",",  true,  " \\\\\\ , \\ \\t'\\\\\\'\\ \\t'", vec("\\ ", " \\t\\'\\ \\t"),  true));
	data.push_back(TestCase(",",  true,  " '' \"'\" , \",',' \" ' '", vec("'", ",','   "),              true));
	data.push_back(TestCase(" ",  true,  "foo bar",                   vec("foo", "bar"),                true));
	data.push_back(TestCase(" ",  true,  " foo bar ",                 vec("", "foo", "bar", ""),        true));
	data.push_back(TestCase(" ",  true,  " foo\\ bar\r\n   \\\\   ",  vec("", "foo bar\r\n", "\\", ""), false));
	data.push_back(TestCase(" ",  true,  "' foo\\ bar' test",         vec(" foo\\ bar", "test"),        true));
	data.push_back(TestCase(",",  true,  "\\",                        vec("\\"),                        true));
	data.push_back(TestCase(",",  true,  "\\ ",                       vec(" "),                         true));
	data.push_back(TestCase(",",  true,  " \\",                       vec("\\"),                        true));
	data.push_back(TestCase(",",  true,  "\\ ",                       vec(" "),                         false));
	data.push_back(TestCase(",",  true,  " \\",                       vec(" \\"),                       false));
	data.push_back(TestCase(",",  true,  "\\\"\\t\\\\\\ \\,\\\\\\ \\\\\\t",   vec("\"\\t\\ ,\\ \\\\t"),         true));
	data.push_back(TestCase(",",  true,  "'\\\"\\t\\\\\\ \\,\\\\\\ \\\\\\t'", vec("\\\"\\t\\\\ \\,\\\\ \\\\t"), true));

	vector<string> result;
	for ( TestData::const_iterator it = data.begin(); it != data.end(); ++it ) {
		splitExt(result, it->source, it->delimiter, it->compressOn, true, it->trim);
		BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(),
		                              it->tokens.begin(), it->tokens.end());
	}
}
