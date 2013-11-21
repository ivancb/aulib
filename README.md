# AuLib

AuLib is a set of miscellaneous C++ bits gathered together under a library.
Right now the only actual usable bits are the IdPool and Event classes.

## Event

Acts as an event source, allowing a user to register several listeners and later on trigger them with event data.
- AddListener - registers a callback as a new listener with the specified priority. Returns the listener's id.
- RemoveListener - removes the specified listener.
- Trigger - immediatly triggers all listeners for this event with the specified data.
- Defer - defers an event to be triggered later on, if the event has a registered DeferredEventContext, otherwise has the same behavior as Trigger.
- RegisterContext - registers the event in the specified DeferredEventContext.

Depends on: IdPool, std::tuple, std::vector
Notes: Not threadsafe. Requires C++11 support for variadic templates. Events can be defer by using an DeferredEventContext. See samples\EventSample for usage examples.

## IdPool

Represents a reusable pool for ids with the following functions:
- Get - returns the next available id (or a previously freed one).
- Free - frees an id so it can be reused.
- Reset - resets the pool state.

Depends on: std::vector
Notes: Not threadsafe.

## Development

AuLib is published under the terms of the MIT License.
Copyright 2013, Ivan Cebola