/*
 * Your task is to take these partial results and make them "nice" by removing all unnecessary parentheses.
 Input

 There is a single positive integer T on the first line of input (equal to about 10000).
 It stands for the number of expressions to follow.
 Each expression consists of a single line containing only
 lowercase letters, operators (+, -, *, /) and parentheses (( and )).

 The letters are variables that can have any value, operators and parentheses have their usual meaning.
 Multiplication and division have higher priority then subtraction and addition.
 All operations with the same priority are computed from left to right (operators are left-associative).
 There are no spaces inside the expressions. No input line contains more than 250 characters.
 *
 * */


#include <boost/tr1/regex.hpp>
#include <iterator>
#include <iostream>
#include <string>

#include <boost/xpressive/xpressive.hpp>

//using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;


using boost::smatch;
using boost::regex;
using boost::regex_search;
using boost::basic_regex;

string binary_op_regex = "(\\({1}.*\\){1}|[a-z])(\\+*\\-*\\**\\\\*)(\\({1}.*\\){1}|[a-z])";


/**
 * Try to parse first occurence of reg_exp
 * */
bool tryOutputFirstGroup(const string& input_, const string& reg_exp, string& output)
{
	regex expr(reg_exp);
	smatch match;
	if(regex_search(input_, match, expr))
	{
		if(match.size() > 1){
			output = match[1];
			return true;
		}
	}
	return false;
}

/**
 * Parses only one occurence
 * */
bool recursiveUnaryApply(const string& input_, const string& reg_exp, string& output)
{
	string tmp_output = input_;
	bool has_changed = false;
	while(tryOutputFirstGroup(tmp_output, reg_exp, tmp_output))
	{
		has_changed = true;
	}
	if(has_changed) output = tmp_output;
	return(has_changed);
}

bool removeSideBrackets(const string& input_, string& output, bool keep_one_ = false)
{
//	string brackets_regex = (keep_one_ == true) ? "^\\({2}(.*)\\){2}$" : "^\\({1}(.*)\\){1}$";
	string brackets_regex = "^\\({1}(.*)\\){1}$";
	bool successful = (recursiveUnaryApply(input_, brackets_regex, output)) ? true : false;
	return successful;
}

void removeBrackets(const string& input_, string& output, bool keep_one_ = false)
{
	if(removeSideBrackets(input_, output, keep_one_))
	{
		;
	} else {
		output = input_;
	}
}

struct ParseTree
{
	string opr;
	string unparsed_content;
	string parsed_content;

	shared_ptr<ParseTree> left;
	shared_ptr<ParseTree> right;


	/**
	 * Initialization of parser
	 * */
	ParseTree(const string& unparsed_content) : unparsed_content(unparsed_content) {};


	inline bool isBracketNeeded(const string& opr)
	{
		if( opr == "+" || opr == "-")
		{
			return false;
		} else return true;

	}

	void parseSelf()
	{
		string unparsed_wo_brackets;
		removeBrackets(unparsed_content, unparsed_wo_brackets);
		vector<string> triple = parse(unparsed_wo_brackets, binary_op_regex);
//		triple.shrink_to_fit();
		/*DEBUG*/
		cout << unparsed_wo_brackets << endl;
//		cout << triple.size() << endl;
//		for( auto t : triple)
//			cout<< t << " "<<endl;
		/* END DEBUG */
		if(triple.size() != 3 )
		{
			throw std::invalid_argument("Cannot parse tree");
			std::terminate();
		}

		opr = triple[1];


		bool needBracket = isBracketNeeded(opr);

		removeBrackets(triple[0], triple[0], needBracket);
		removeBrackets(triple[2], triple[2], needBracket);

		left = make_shared<ParseTree>(ParseTree(triple[0]));
		right = make_shared<ParseTree>(ParseTree(triple[2]));
	}

	static vector<string> parse(const string& input_, const string& reg_exp)
	{
		regex expr(reg_exp);

		vector<string> groups;
		groups.reserve(3);

		smatch match;
		if(regex_search(input_, match, expr))
		{
			for(unsigned i = 1; i <= match.size(); i++)
			{
				/*DEBUG*/
				cout << "DEBUG: {" << match[i] << "}" << endl;
				if(match[i] != "")/*Sometimes regex_search shows empty string as a groupped match*/
				{
					groups.emplace_back(match[i]);
				}
			}
		}
		return groups;
	}

	void print()
	{
		 cout << unparsed_content << " -> " << left->unparsed_content << "{" << opr << "}"<< right->unparsed_content  << endl ;
	}

};

int main()
{
//	string tst = "(((a+b)))";
//	string parsed;
//	if(removeSideBrackets(tst, parsed))
//		cout << tst << " -> "<< parsed << endl;
//	else
//		cout << "not found" << endl;

//	string parsed1;
//	removeBrackets(tst, parsed1);
//	cout << parsed1 << endl;

//	string tst2 = "(a+b)+c";
//	string reg_ex = "(\\({1}.*\\){1}|[a-z])(\\+*\\-*\\**\\\\*)(\\({1}.*\\){1}|[a-z])";
//	vector<string> parsed2 = ParseTree::parse(tst2, reg_ex);
//	cout << tst2 << " -> ";
//	for(auto& p : parsed2) cout << "{" << p << "}" ;
//	cout << endl;

//	terminate called after throwing an instance of
//	'boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::regex_error> >'
//	  what():  Found a closing repetition operator } with no corresponding {.
//
//		  The error occurred while parsing the regular expression fragment: ']+|(?R))*)>>>HERE>>>}'.

//	string tst2 = "(a+b)+(c)";
//	string reg_ex = "\\(((?>[^\\(\\)]+|(?R))*)\\)";
//	vector<string> parsed2 = ParseTree::parse(tst2, reg_ex);
//	cout << tst2 << " -> ";
//	for(auto& p : parsed2) cout << "{" << p << "}" ;
//	cout << endl;


////	auto pt = ParseTree("((a+b)+c)"); //((a+b)+c) -> a+b{+}c
//	auto pt = ParseTree("(a+b)+(c)");
//	pt.parseSelf();
//	pt.print();

	return 0;
}

//(\((?:\1??[^\(]*?\)))+



//#include <string>
//#include <iostream>
//
//int main()
//{
//  std::string s = "Boost Libraries";
//  boost::regex expr{"\\w+\\s\\w+"};
//  std::cout << std::boolalpha << boost::regex_match(s, expr) << '\n';
//}


