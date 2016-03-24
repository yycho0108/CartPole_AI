/* ** */
#include "GameManager.h"

using namespace std;

int main(){
	GameManager<4,4> g("kbd");
	g.run();

	/*
	Board<4,4> b; //up to 16 supported by current implementation
	for(int i=0;i<4;++i){
		for(int j=0;j<4;++j){
			b.set(i,j,1);
		}
	}
	Board<4,4> c(b.repr());

	c.print();
	c.next(DIR::D);
	c.print();

	cout << "B : " << b.repr() << endl;
	cout << "C : " << c.repr() << endl;
	*/
}
