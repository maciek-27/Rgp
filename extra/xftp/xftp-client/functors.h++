
template < class ON, class OFF >
class ToggleFunctor
{
	ON on;
	OFF off;
public:
	ToggleFunctor(ON on_, OFF off_)
		: on(on_), off(off_) {}

	void operator() (Tokenizer &	 optns)
		{
			using namespace boost::lambda;
			using namespace std;
			string val;
			Tokenizer::iterator i = optns.begin ();
			if (i!=optns.end ())
				val.assign (*i);
			
			if (val=="yes")
				on();
			else if (val=="no")
				off();
			else
				cout << "yes? no?" << endl;
		}
};

template < class ON, class OFF >
ToggleFunctor<ON,OFF>
Toggle(ON on, OFF off)
{
	ToggleFunctor<ON,OFF> result(on,off);
	return result;
}



template < class F >
class PrintFunctor
{
	F f;
public:
	PrintFunctor(F f_)
		: f(f_) {}

	void operator() (Tokenizer &	 optns)
		{
			using namespace std;
			cout << f() << endl;
		}
};

template < class F >
PrintFunctor<F>
Print(F f)
{
	PrintFunctor<F> result(f);
	return result;
}




template < class F >
class SetFunctor
{
	F f;
public:
	SetFunctor(F f_)
		: f(f_) {}

	void operator() (Tokenizer &	 optns)
		{
			using namespace std;
			Tokenizer::iterator i = optns.begin ();
			if (i!=optns.end ())
				f (*i);
			else
				cout << "huh?" << endl;
		}
};

template < class F >
SetFunctor<F>
Set(F f)
{
	SetFunctor<F> result(f);
	return result;
}
