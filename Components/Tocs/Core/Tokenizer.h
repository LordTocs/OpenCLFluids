#pragma once
#include <string>
#include "SpecialTokenSet.h"
namespace Tocs {
namespace Lexing {

class TokenType
{
	enum InternalEnum
	{
		word,
		stringliteral,
		integer,
		_float,
		symbol,
		comment,
		multilinecomment
	};
	InternalEnum Internal;

	TokenType (InternalEnum format)
		: Internal (format) {}

public:
	const static TokenType Word;
	const static TokenType Integer;
	const static TokenType Float;
	const static TokenType StringLiteral;
	const static TokenType Symbol;
	const static TokenType Comment;
	const static TokenType MultiLineComment;

	bool operator ==  (const TokenType &op2) const {return Internal == op2.Internal;}
	bool operator !=  (const TokenType &op2) const {return Internal != op2.Internal;}

	std::string ToString () const;
};


class CharSource
{
public:
	virtual char GetChar () = 0;
	virtual char PeekChar () const = 0;
	virtual bool EndOfSource () const = 0;
};

class StringSource : public CharSource
{
	int Index;
	std::string String;
public:
	StringSource (const std::string &str)
		: Index(0), String(str) {}

	char GetChar ()
	{ return String[Index++]; }
	
	char PeekChar () const
	{ return String[Index];	}

	bool EndOfSource () const
	{ return Index >= String.length (); }

	int GetIndex () const 
	{ return Index;	}

	std::string &GetString ()
	{ return String; }

	static StringSource FromFile(const std::string &file);

};

class Tokenizer
{
	CharSource &Source;
	
	std::string Token;

	void LexNext ();
	bool End;
public:
	Tokenizer(CharSource &chars)
		: Source(chars),
		  End(false)
	{ LexNext(); }
	bool EndOfStream ()
	{ return End; }

	std::string GetToken ()
	{
		std::string temp = Token;
		LexNext ();
		return temp;
	}
	std::string PeekToken ()
	{
		return Token;
	}

	bool Is(const std::string &expectedtoken)
	{
		if (Token == expectedtoken)
		{
			LexNext ();
			return true;
		}

		return false;
	}

	bool FindClose (const std::string &open, const std::string &close);
};

}}