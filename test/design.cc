#include "../util/util.h"

#include "../design/observer.h"
#include "../design/strategy.h"
#include "../design/factory.h"

namespace test {

DEF_test(Observer) {
	design::Topic topic;

	design::Subscriber sub01("sub01");
	design::Subscriber sub02("sub02");
	design::Subscriber sub03("sub03");

	topic.attach(&sub01);
	topic.attach(&sub02);
	topic.attach(&sub03);

	topic.notify();
}

DEF_test(Strategy) {
	design::Swing swing;
	design::Run run;
	design::Jump jump;

	design::Strategy<design::Swing> context_swing(swing);
	context_swing.action();

	design::Strategy<design::Run> context_run(run);
	context_run.action();

	design::Strategy<design::Jump> context_jump(jump);
	context_jump.action();

	//使用指针的方式可以动态变化策略
	design::StrategyPointer<design::Action> context;
	context.setStrategy(&swing);
	context.action();

	context.setStrategy(&run);
	context.action();

	context.setStrategy(&jump);
	context.action();
}

DEF_test(Factory) {
	design::Factory1 f1;

	design::A1 *a1 = (design::A1 *)f1.createProductA("a1");
	design::B1 *b1 = (design::B1 *)f1.createProductB("b1");
	a1->perform();
	b1->perform();

	design::Factory2 f2;
	design::A2 *a2 = (design::A2 *)f2.createProductA("a2");
	design::B2 *b2 = (design::B2 *)f2.createProductB("b2");
	a2->perform();
	b2->perform();
}


}