#include <cstdio>
#include <au/Event.h>

void SimpleEvents()
{
	printf("Simple events\n");

	au::Event<> zeroParamEvent;
	au::Event<int, bool, char*, int> multipleParamEvent;

	zeroParamEvent.AddListener(0, []() { printf("\tZero Param Event triggered\n"); return au::EventCallbackResult::Ok; });
	multipleParamEvent.AddListener(0, [](const int a, const bool b, const char* c, const int d) 
	{ 
		printf("\tMultiple parameters (a=%d, b=%s, c=%s, d=%d)\n", a, b ? "true" : "false", c, d);
		return au::EventCallbackResult::Ok;
	});

	zeroParamEvent.Trigger();
	multipleParamEvent.Trigger(15, true, "hello", 3);
	multipleParamEvent.Trigger(53, false, "world", 0);
}

void MultipleListeners()
{
	printf("\nEvent priorities\n");

	au::Event<int, int> evt;

	evt.AddListener(0, [](const int a, const int b) { printf("\tFirst listener triggered (a=%d b=%d)\n", a, b); return au::EventCallbackResult::Ok; });
	int lidRemove = evt.AddListener(0, [](const int a, const int b) { printf("\tSecond listener triggered (a=%d b=%d)\n", a, b); return au::EventCallbackResult::Ok; });
	evt.AddListener(0, [](const int a, const int b) { printf("\tThird listener triggered (a=%d b=%d)\n", a, b); return au::EventCallbackResult::Ok; });

	evt.Trigger(5, 3);

	printf("Removing listener %d\n", lidRemove);
	evt.RemoveListener(lidRemove);
	evt.Trigger(1, 4);
}

void EventPriorities()
{
	printf("\nEvent priorities\n");

	au::Event<> evt;

	evt.AddListener(0, []() { printf("\tFirst listener triggered (p=0)\n"); return au::EventCallbackResult::Ok; });
	evt.AddListener(10, []() { printf("\tSecond listener triggered (p=10)\n"); return au::EventCallbackResult::Ok; });
	evt.AddListener(5, []() { printf("\tThird listener triggered (p=5)\n"); return au::EventCallbackResult::Ok; });

	evt.Trigger();
}

void DeferredEvents()
{
	printf("\nEvent priorities\n");

	au::DeferredEventContext defContext;
	au::Event<int> evt;
	evt.RegisterContext(&defContext);

	evt.AddListener(0, [](const int cv) { printf("\tListener triggered (v = %d)\n", cv); return au::EventCallbackResult::Ok; });

	evt.Defer(15); 
	evt.Defer(50);
	evt.Defer(100);

	printf("Events deferred, now triggering an event\n");
	evt.Trigger(0);

	printf("Triggering deferred context\n");
	defContext.Trigger();
}

int main(int argc, char *argv[])
{
	SimpleEvents();
	MultipleListeners();
	EventPriorities();
	DeferredEvents();

	getchar();
	return 0;
}