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


enum class ExprType {
	Container,
	Brackets, /*  */
	Constant, /* represents letters */
	Operator, /* + - * / */
	Empty
};

std::ostream& operator << (std::ostream& os, const ExprType& obj)
{
   os << static_cast<std::underlying_type<ExprType>::type>(obj);
   return os;
}

void wrapin_brackets(string& str)
{
	str += ")";
	str.insert (0, 1, '(');
}

struct ExprContainer
{
	ExprType type;
	string str;
	vector<shared_ptr<ExprContainer>> children;

	/* This variable wraps children in brackets*/
	bool is_extract_with_brackets = false;

	ExprContainer(ExprType type_, const string str_) : type(type_), str(str_) {};

	ExprContainer(string str = "") : ExprContainer(ExprType::Container, str) {};

	void addChild(shared_ptr<ExprContainer>& chld_ptr)
	{
		children.emplace_back(chld_ptr);
	}
	shared_ptr<ExprContainer> addChild(ExprType type_, string str_)
	{
		shared_ptr<ExprContainer> chld = make_shared<ExprContainer>(type_, str_);
		children.emplace_back(chld);
		return(chld);
	}

	void addChild(ExprType type_, const char& chr_)
	{
		addChild(type_, string(1, chr_));
	}

	void print(int print_offset_ = 0, int child_offset_ = 2)
	{
		string offset = std::string(print_offset_, ' ');
		cout << offset << this->type << " : " << str << "." << endl;
		for (auto& p : children)
			p->print(print_offset_ + child_offset_);
	}

	void removeBrackets()
	{
		if(this->type == ExprType::Brackets)
		{
			str.pop_back();
			str.erase(str.begin());
		} else {
			throw std::logic_error("Brackets can be only removed for brackets container");
		}
	}

	bool hasChildren()
	{
//		return children.size() > 1;
		return !children.empty();
	}

	string extract()
	{
		string extract;
		if(!hasChildren())
		{
			return str;
		}
		else{
			if(is_extract_with_brackets) extract += '(';
			for (const auto& p : children)
			{
				extract += p->extract();
			}
			if(is_extract_with_brackets) extract += ')';
		}
		return extract;
	}


};

/*
 * Iterator -> (ExprContainer, Iterator) ->
 * */


/* we proceed with tail and fill cont, if we find new cont   */
string get_between_brackets(string::iterator& tail_, const string::iterator& end_)
{
	string brackets_content;
	if( (end_ - tail_) >= 2)
	{
		uint br_cnt = 0;
		while(tail_ != end_)
		{
			const auto& chr = *tail_;
			switch(chr)
			{
				case '(': br_cnt++; break;
				case ')': br_cnt--; break;
			}
			if(br_cnt >= 0)
				brackets_content.push_back(chr);

			if (br_cnt == 0)
			{
				break;
			}
			++tail_;
		}
	} else throw std::invalid_argument("brackets tail is not long enough");
	return brackets_content;
}

inline bool is_bracket(const char& chr)
{
	return (chr == '(' || chr == ')');
}

inline bool is_operator(const char& chr)
{
	return (chr == '*' || chr == '+' || chr == '/' || chr == '-');
}

/* As soon as we hit one of the keywords, we create new container and pass it to fillContainer
 * while container is being filled function moves iterator further; if within tail new structures
 * are found their will be appended inside `str` of `cont`;
 * */
void parse_string(ExprContainer& cont_)
{
	string::iterator start = cont_.str.begin();
	const string::iterator& end = cont_.str.end();

	for(auto& it = start; it != end; ++it)
	{
		const auto& cur_chr = *it;
//		cout << "DEBUG:" << cur_chr << endl;
		if(isspace(cur_chr))
		{
			cout << "DEBUG parse_string: found space"<< endl;
			continue;
		} else if(isalpha(cur_chr))
		{
			cout << "DEBUG parse_string: found alpha: " << cur_chr << endl;
			cont_.addChild(ExprType::Constant, cur_chr);
		} else if (is_operator(cur_chr))
		{
			cout << "DEBUG parse_string: found operator: " << cur_chr << endl;
			cont_.addChild(ExprType::Operator, cur_chr);
		} else if (is_bracket(cur_chr))
		{
			cout << "DEBUG parse_string: found bracket: " << cur_chr << endl;
			auto txt_in_brackets = get_between_brackets(it, end);
			shared_ptr<ExprContainer> bracketsCont = make_shared<ExprContainer>(ExprType::Brackets, txt_in_brackets);
			bracketsCont->removeBrackets();
			parse_string(*bracketsCont);
			if(!bracketsCont->hasChildren())
			{
				cont_.addChild(ExprType::Empty, "");
			}
			else
			{
				cont_.addChild(bracketsCont);
			}
		}
	}
}


void put_brackets(ExprContainer& input_cont_)
{
	for(auto it = input_cont_.children.begin(); it != input_cont_.children.end(); ++it)
	{
		ExprContainer& chld = **it;
		if(chld.hasChildren())
		{
			put_brackets(chld);
		} else {

			cout << "DEBUG put_brackets: chld = " << chld.str << endl;

			if(chld.type == ExprType::Operator)
			{
				string& op = chld.str;
				if(op == "*" || op == "/")
				{
					auto& prev_chld = **(it-1);
					auto& next_chld = **(it+1);//TODO check that expression is correct? Might be reaching end to soon
					prev_chld.is_extract_with_brackets = true;
					next_chld.is_extract_with_brackets = true;
				} else if (op == "-")
				{
					auto& next_chld = **(it+1);
					next_chld.is_extract_with_brackets = true;
				}
			}
		}

	}
}

int main()
{

//	string tst = "(a+(b*c))";
//	string tst = "((a+b)*c)
//	string tst = "(a*(b*c))";
	string tst = "(a*(b/c)*d)";
//	string tst = "((a/(b/c))/d)";
//	string tst = "((x))";
//	string tst = "(a+b)-(c-d)-(e/f)";
//	string tst = "(a+b)+(c-d)-(e+f)";


//	string tst = "(d*((f+g)))";
//	string tst = "d*(f+g)";
//	string tst = "(a+b)+(c)+d*(f+g)";
//	string tst = "(a+b+(b+g)+(g*t*(k+b))+ ())";
//	wrapin_brackets(tst);

	cout << tst << endl;

	ExprContainer top(tst);
	parse_string(top);
	top.print();

	put_brackets(top);
	string extract = top.extract();

	cout << extract << endl;

	return 0;
}
