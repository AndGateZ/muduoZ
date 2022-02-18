#include<boost/operators.hpp>
#include<iostream>

class point :public boost::less_than_comparable<point>//如果把这里的继承删除，else if(t1>t2)就会找不到>的运算符重载
{
public:
	point(int num = 0) :num_(num){}
	int getNum()const{ return num_; }
 
private:
	int num_;
};

inline bool operator<(const point &lhs,const point &rhs)
{
    std::cout << "operator<" << std::endl;
	return lhs.getNum() < rhs.getNum();
}

inline bool operator==(const point &lhs, const point &rhs)
{
    std::cout << "operator==" << std::endl;
	return lhs.getNum() == rhs.getNum();
}

int main()
{
	point t1(10);
	point t2;
	if (t1 == t2)
	{
		std::cout << "t1 == t2" << std::endl;
	}
    else if(t1<t2){
        std::cout << "t1 < t2" << std::endl;
    }
    else if(t1>t2){
        std::cout << "t1 > t2" << std::endl;
    }
	return 0;
}
