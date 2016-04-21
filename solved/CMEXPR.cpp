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

#include <iterator>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;

using std::distance;
using std::advance;
using std::prev;
using std::next;


enum class ExprType {
	Container,
	Brackets,
	Constant,
	Operator,
	Empty
};
/*
 * For example, subtraction and division, as used in conventional math notation, are inherently left-associative.
 * Addition and multiplication, by contrast, are both left and right associative.
 * */
enum class OpType {
	NA = 0,
			Sum = 1,
			Minus = 2,
			Product = 3,
			Div = 4
};
std::ostream& operator << (std::ostream& os, const ExprType& obj)
{
	os << static_cast<std::underlying_type<ExprType>::type>(obj);
	return(os);
}
std::ostream& operator << (std::ostream& os, const OpType& obj)
{
	os << static_cast<std::underlying_type<OpType>::type>(obj);
	return(os);
}

/* Misc functions*/
void wrapin_brackets(string& str)
{
	str += ")";
	str.insert (0, 1, '(');
}

/* returns string with evertything between matching brackets with brackets included */
string get_between_brackets(string::iterator& tail_, const string::iterator& end_)
{
	string brackets_content;
	if(distance(tail_, end_) >= 2)
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
			advance(tail_, 1);
		}
	} else throw std::invalid_argument("brackets tail is not long enough");
	return(brackets_content);
}
inline bool is_operator(const char& chr)
{
	return (chr == '*' || chr == '+' || chr == '/' || chr == '-');
}
struct ExprContainer
{
	ExprType type;
	string str;
	vector<shared_ptr<ExprContainer>> children;
	/* This variable wraps children in brackets*/
	bool extr_brackets = false;
	OpType operator_type = OpType::NA;
	bool isset_operator_inside = false;
	OpType operator_inside = operator_type;

	int left_prio_dbg  = 0;
	int opr_prio_dbg   = 0;
	int right_prio_dbg = 0;


	/* CONSTRUCTORS */
	ExprContainer(ExprType type_, const string str_) : type(type_), str(str_) {
		if(type_ == ExprType::Operator)
		{
			switch(str_.front())
			{
			case '+': this->operator_type = OpType::Sum; break;
			case '-': this->operator_type = OpType::Minus; break;
			case '*': this->operator_type = OpType::Product; break;
			case '/': this->operator_type = OpType::Div; break;
			throw std::invalid_argument("input is not an operator");
			}
		}
	};
	ExprContainer(string str = "") : ExprContainer(ExprType::Container, str) {};
	/* Adding children */
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
	inline bool hasChildren()
	{
		return(!children.empty());
	}
	inline bool hasOneChild()
	{
		return(children.size() == 1);
	}
	inline bool hasMoreThanOneChild()
	{
		return(children.size() > 1);
	}

	/* @recursive
	 * If has children with more than one element inside
	 * */
	bool hasChildrenRecursive()
	{
		if(this->hasChildren())
		{
			if(this->children.size() > 1)
			{
				return(true);
			} else {
				for(const auto & chld : this->children)
				{
					if(chld->hasChildrenRecursive()) return(true);
				}
			}
		}
		return(false);
	}
	/* Operation inside (0 or 1 level down);
	 * */
	OpType opTypeInside()
	{
		if(this->isset_operator_inside)
		{
			return(this->operator_inside);
		} else if(hasOneChild()) {
			return(setOpTypeInside(children.front()->opTypeInside()));
		} else if(hasMoreThanOneChild()) {
			OpType min_op_type = OpType::NA;
			/* Scan all childern and find minimim operator from all included, but bigger than NA*/
			for(auto& chld : children)
			{
				ExprContainer& chldref = (*chld);
//				auto op_type = chldref.opTypeInside();
				auto op_type = chldref.operator_type;/* No recursion, because we just looking on one level below*/
				if(op_type == OpType::NA)
				{
					continue;
				} else if(min_op_type == OpType::NA || op_type < min_op_type){
					min_op_type = op_type;
				}
			}
			return(setOpTypeInside(min_op_type));
		} else {
			return(this->operator_type);
		}
	}

	OpType setOpTypeInside(const OpType op)
	{
		this->operator_inside = op;
		this->isset_operator_inside = true;
		return(op);
	}

	/* Brackets related */
	void setBrackets()
	{
		if( (this->type == ExprType::Brackets && !this->hasChildrenRecursive()))
		{
			;
		} else {
			this->extr_brackets = true;
		}
	}
	void removeBrackets()
	{
		if(this->type == ExprType::Brackets)
		{
			auto detect_bracket = [](char x){ return(')' == x || '(' == x);};
			if(detect_bracket(str.back()))
				str.pop_back();

			if(detect_bracket(str.front()))
				str.erase(str.begin());
		} else {
			throw std::logic_error("Brackets can be only removed for brackets container");
		}
	}
	/* Outputs */
	void print(int print_offset_ = 0, int child_offset_ = 2)
	{
		string offset = std::string(print_offset_, ' ');

		string brackets = "";
		if(this->extr_brackets) brackets = "     ->  ((";

		cout << "DEBUG: " << offset << this->type << " : " << str;
		if(this->hasChildren() || this->type == ExprType::Operator)
		{
			cout << " op " << this->opTypeInside() << brackets  <<
					" prio " << left_prio_dbg << opr_prio_dbg << right_prio_dbg;
		}
		cout << endl;
		for (auto& p : children)
			p->print(print_offset_ + child_offset_);
	}
	string extract()
	{
		string extract;
		if(hasChildren())
			//		if(this->hasChildrenRecursive())
		{
			bool with_brackets = this->extr_brackets;
			if(with_brackets) extract += '(';
			for (const auto& p : children)
			{
				extract += p->extract();
			}
			if(with_brackets) extract += ')';
		}
		else{
			return(str);
		}
		return(extract);
	}
};
//////////////////////////////////////////////////
inline void parse_string(ExprContainer& cont_)//FIXME if the container is just wrapper, need to delete itermediary level
{
	const string::iterator& end = cont_.str.end();
	auto it = cont_.str.begin();
	while (it != end)
	{
		const auto& cur_chr = *it;
		//		cout << "DEBUG: " << cur_chr << endl;
		if(isspace(cur_chr))
		{
			continue;
		} else if(isalpha(cur_chr))
		{
			cont_.addChild(ExprType::Constant, cur_chr);
		} else if (is_operator(cur_chr))
		{
			cont_.addChild(ExprType::Operator, cur_chr);
		} else if (cur_chr == '(') {
			auto txt_in_brackets = get_between_brackets(it, end);
			shared_ptr<ExprContainer> bracketsCont = make_shared<ExprContainer>(ExprType::Brackets, txt_in_brackets);
			bracketsCont->removeBrackets();//FIXME removes non-bracket from end!
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
		if(it == end){
			break;
		} else {
			std::advance(it, 1);
		}
	}
}
//////////////////////////////////////////////////
/*                       +  -  *  /    */
int left_prios[5]  = {0, 3, 3, 5, 5};
int opr_prios[5]   = {0, 2, 3, 4, 5};
int right_prios[5] = {0, 2, 2, 4, 4};

inline int opr_prio(OpType operat)
{
	return(opr_prios[(int)operat]);
}
inline int left_prio(OpType operand)
{
	return(left_prios[(int)operand]);
}
inline int right_prio(OpType operand)
{
	return(right_prios[(int)operand]);
}

//std::tuple<bool, bool>
//which_to_wrap(OpType opr, OpType left, OpType right)
//{
//	bool towrapleft = false;
//	bool towrapright = false;
//	int opr_pr = opr_prio(opr);
//
//	if(opr_pr > left_prio(left)) {
//		towrapleft = true;
//	}
//	if(opr_pr > right_prio(right)) {
//		towrapright = true;
//	}
//	return(std::make_tuple(towrapleft, towrapright));
//}

std::tuple<bool, bool>
which_to_wrap(shared_ptr<ExprContainer>& opr, shared_ptr<ExprContainer>& left, shared_ptr<ExprContainer>& right)
{
	bool towrapleft = false;
	bool towrapright = false;

	OpType opr_op = (opr != nullptr) ? opr->opTypeInside() : OpType::NA;
	OpType left_op  = (left != nullptr) ? left->opTypeInside() : OpType::NA;
	OpType right_op = (right != nullptr) ? right->opTypeInside() : OpType::NA;

	int opr_pr = opr_prio(opr_op);
	int left_pr = left_prio(left_op);
	int right_pr = right_prio(right_op);

	/*DEBUG*/
	if(opr != nullptr) opr->opr_prio_dbg = opr_pr;
	if(left != nullptr) left->left_prio_dbg = left_pr;
	if(right != nullptr) right->right_prio_dbg = right_pr;

	if(opr_pr > left_pr) {
		towrapleft = true;
	}
	if(opr_pr > right_pr) {
		towrapright = true;
	}
	return(std::make_tuple(towrapleft, towrapright));
}
//////////////////////////////////////////////////

void mark_brackets(ExprContainer& input_cont_)
{
	for(auto opr_it = input_cont_.children.begin(); opr_it !=  input_cont_.children.end(); ++opr_it)
	{
		auto& curr_opr = (*opr_it);
		if(curr_opr->hasChildren())
		{
			mark_brackets(*curr_opr);
		} else if (curr_opr->type == ExprType::Operator) {

			auto& prev_chld = *(opr_it-1);
			auto& next_chld = *(opr_it+1);

			bool left_brackets = false;
			bool right_brackets = false;
			std::tie(left_brackets, right_brackets) = which_to_wrap(curr_opr, prev_chld, next_chld);

			bool is_missing_prev = (prev_chld == nullptr);
			if(left_brackets == true && !is_missing_prev)
				prev_chld->setBrackets();
			if(right_brackets == true)
				next_chld->setBrackets();

		}
	}
}

string remove_excess_brackets(string& str)
{
	ExprContainer top(str);
	parse_string(top);
	mark_brackets(top);
	string extract = top.extract();
	return(extract);
}

///////////////////////////////////////////////
bool test(string tst, string expect, bool verbose_ = true)
{
	ExprContainer top(tst);
	parse_string(top);
	mark_brackets(top);
	string extract = top.extract();
	if(verbose_)
	{
		top.print();
		cout << tst << " -> " << extract << endl;
	}
	bool passed = expect == extract;
	if(!passed) cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!    must be " <<  expect << endl;
	return(passed);
}

//#include <gperftools/profiler.h>

int main()
{
	//	ProfilerStart("/home/cracs/CMEXPR.txt");
	//    for(int i = 0; i < 10000; i++)
	//    {
	//    }
	//    ProfilerFlush();
	//    ProfilerStop();

//	test("-(a)"                       , "-a"                                    , false);
//	test("(a+b"                       , "a+b"                                   , false);
//	test("a+b)"                       , "a+b"                                   , false);
//	test("a+(b-c*(d-e))/g"            , "a+(b-c*(d-e))/g"                       , false);
//	test("(a+((b+c+d)/g))"            , "a+(b+c+d)/g"                           , false);
//	test("((a)+(b*c))"                , "a+b*c"                                 , false);
//	test("(a+(b*c))"                  , "a+b*c"                                 , false);
//	test("((a+b)*c)"                  , "(a+b)*c"                               , false);
//	test("(a*b)/c)"                   , "a*b/c"                                 , false);  /**/
//	test("(a*(b*c))"                  , "a*b*c"                                 , false);
//	test("(a*(b/c)*d)"                , "a*b/c*d"                               , false);  /**/
//	test("((a/(b/c))/d)"              , "a/(b/c)/d"                             , false);  /**/
//	test("((x))"                      , "x"                                     , false);
//	test("(a+b)-(c-d)-(e/f)"          , "a+b-(c-d)-e/f"                         , false);
//	test("(a+b)+(c-d)-(e+f)"          , "a+b+c-d-(e+f)"                         , false);
//	test("((((((a)+b)+c)+d)*e)+f)*g"  , "((a+b+c+d)*e+f)*g"                     , false);
//	test("a+(b-c*(d-e)/f)/g"          , "a+(b-c*(d-e)/f)/g"                     , false);
//	test("a*b+(b-b+b)","a*b+b-b+b"  , false);
//	test("a*b/(a-c*c)","a*b/(a-c*c)", false);
//	test("b+b*(a+a-b)","b+b*(a+a-b)", false);
//	test("c/c*(b+a*a)","c/c*(b+a*a)", false);
//	test("a+c+(a+a+a)","a+c+a+a+a"  , false);
//	test("b-c*(a*c-a)","b-c*(a*c-a)", false);
//	test("a/c-(c*b/b)","a/c-c*b/b"  , false);
//	test("c+a*(b/c/c)","c+a*b/c/c"  , false);
//
//	test("a+(b+c)/(d+((f+(k+m))+n))","a+(b+c)/(d+f+k+m+n)"    , false);
//	test("(a-b-c+(d+(((f/k+m))/n)))","a-b-c+d+(f/k+m)/n"      , false);
//	test("(a/b+(c-d)/(((f)-k))-m+n)","a/b+(c-d)/(f-k)-m+n"    , false);
//	test("a-(b+(c/d+f)-(k)/((m+n)))","a-(b+c/d+f-k/(m+n))"    , false);
//	test("a-(b+(c-((d/f)+(k)+m))+n)","a-(b+c-(d/f+k+m)+n)"    , false);
//	test("(a/b/(c-d)/((f/k))/(m)-n)","a/b/(c-d)/(f/k)/m-n"    , false);
//	test("(a+((b/c)/d+(f)+k/(m))/n)","a+(b/c/d+f+k/m)/n"      , false);
//	test("(((a+b+(c)/d)/(f)/k)/m)/n","(a+b+c/d)/f/k/m/n"      , false);
//	test("a+b+c+(d-((f-(k))+(m+n)))","a+b+c+d-(f-k+m+n)"      , false);
//	test("(a/(b+(c+((d)/f)/k))/m)-n","a/(b+c+d/f/k)/m-n"      , false);
//	test("(a-b/(c/(d)/(f)-(k-m)/n))","a-b/(c/d/f-(k-m)/n)"    , false);
//	test("a+((b/c+d))+f+(k/((m/n)))","a+b/c+d+f+k/(m/n)"      , false);
//	  test("a+b+c+(((d-f/(k/(m)))-n))","a+b+c+d-f/(k/m)-n"      , false);
//	  test("((a+b)-c-d-f+k)-(((m+n)))","a+b-c-d-f+k-(m+n)"      , false);
//	  test("(a+b+c)-(d-((f+(k))+m))+n","a+b+c-(d-(f+k+m))+n"    , false);
//	  test("(a+(b+(c)+(d-f)+k/(m)-n))","a+b+c+d-f+k/m-n"        , false);
//	  test("(a+((b/c/d-f+k)+((m))/n))","a+b/c/d-f+k+m/n"        , false);
//	  test("(a+b)/(((c/(d/f+k))+m-n))","(a+b)/(c/(d/f+k)+m-n)"  , false);
//	  test("a+(b+c)+d-f+(((k+(m)))+n)","a+b+c+d-f+k+m+n"        , false);
//	  test("a-b/((c+(d)/(f)+k+(m+n)))","a-b/(c+d/f+k+m+n)"      , false);
//	  test("(a+(b-(c+(d+(f/k))+m)/n))","a+b-(c+d+f/k+m)/n"      , false);
//	  test("((a-b+c)+d-((f-(k)+m)+n))","a-b+c+d-(f-k+m+n)"      , false);
//	  test("(a-((b-c+d))+(f-(k-m))/n)","a-(b-c+d)+(f-(k-m))/n"  , false);
//	  test("(a-(b+c)+d)/((f+k+(m)-n))","(a-(b+c)+d)/(f+k+m-n)"  , false);
//	  test("(a+b-(c+(d)-f)-(k/(m)/n))","a+b-(c+d-f)-k/m/n"      , false);
//	  test("(a+(b+c)-(d/((f)+k+m)-n))","a+b+c-(d/(f+k+m)-n)"    , false);
//	  test("a+b/(c+d/((f+(k))/(m+n)))","a+b/(c+d/((f+k)/(m+n)))", false);
//	  test("(a-b+((c/(d)+(f+k))+m)+n)","a-b+c/d+f+k+m+n"        , false);
//	  test("(a+b-((c+(d)+f)/(k/m)+n))","a+b-((c+d+f)/(k/m)+n)"  , false);
//	  test("a+b+c+(d-f-(((k)+(m))/n))","a+b+c+d-f-(k+m)/n"      , false);
//	  test("(a+(((b-(c)+d)+f)-k)+m+n)","a+b-c+d+f-k+m+n"        , false);
//	  test("a-b+c/(d+(f-k/(((m-n)))))","a-b+c/(d+f-k/(m-n))"    , false);
//	  test("a+((b+c+d+(f)+(k)+(m)+n))","a+b+c+d+f+k+m+n"        , false);
//	  test("a/b+c+(d+(f)/(k+((m)/n)))","a/b+c+d+f/(k+m/n)"      , false);
//	  test("(a+(b+(c/d+(f))+(k-m))+n)","a+b+c/d+f+k-m+n"        , false);
//	  test("(a-b+c-d)/((((f-k))-m+n))","(a-b+c-d)/(f-k-m+n)"    , false);
//	  test("a/(b-c+d+(f/(k+((m))+n)))","a/(b-c+d+f/(k+m+n))"    , false);
//	  test("a+((b-(c)+(d)+f)-k)+(m+n)","a+b-c+d+f-k+m+n"        , false);
//	  test("a+b-((c/(d/(f)/(k)))+m)/n","a+b-(c/(d/f/k)+m)/n"    , false);
//	  test("((a+b+(c+((d+f)))/k+m+n))","a+b+(c+d+f)/k+m+n"      , false);
//	  test("(a/b+((((c+d+f))/k+m)/n))","a/b+((c+d+f)/k+m)/n"    , false);
//	  test("a/(b-(c-(d+f-(k)/(m))/n))","a/(b-(c-(d+f-k/m)/n))"  , false);
//	  test("a+(((b+c-d+f)+k)+((m/n)))","a+b+c-d+f+k+m/n"        , false);
//	  test("(a-b+c-d-((f+(k-(m))+n)))","a-b+c-d-(f+k-m+n)"      , false);
//	  test("(((a+b)+c+(d/(f))/k)/m)+n","(a+b+c+d/f/k)/m+n"      , false);
//	  test("a/(b-(c+(d-f+(k)+(m)+n)))","a/(b-(c+d-f+k+m+n))"    , false);
//	  test("a+b-(c+(d-(f)+(k/(m)))+n)","a+b-(c+d-f+k/m+n)"      , false);
//	  test("(a+(b/((c+(d)/f)-k/m+n)))","a+b/(c+d/f-k/m+n)"      , false);
//	  test("(a-((b-c+d)+(f+k))+(m-n))","a-(b-c+d+f+k)+m-n"      , false);
//	  test("((a-b+(c+d+(f/(k+m)/n))))","a-b+c+d+f/(k+m)/n"      , false);
//	cout << endl << endl << "DONE" << endl;


			uint num_of_expr;
			std::cin >> num_of_expr;
			vector<string> unparsed(num_of_expr);
			for(uint i = 0; i < num_of_expr; i++)
			{
				std::cin >> unparsed[i];
				if(std::cin.eof()) break;
			}
			for(auto& unpr : unparsed)
				cout << remove_excess_brackets(unpr) << endl;

			return(0);
}

