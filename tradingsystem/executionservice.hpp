/**
* executionservice.hpp
* Defines the data types and Service for executions.
*
* @author Breman Thuraisingham
* @coauthor Junliang Jimmy Zhou
*/
#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include "algoexecutionservice.hpp"

/**
* Pre-declearations to avoid errors.
*/
template<typename T>
class ExecutionToAlgoExecutionListener;

/**
* Service for executing orders on an exchange.
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class ExecutionService : public Service<string, ExecutionOrder<T>>
{

private:

	map<string, ExecutionOrder<T>> executionOrders;
	vector<ServiceListener<ExecutionOrder<T>>*> listeners;
	ExecutionToAlgoExecutionListener<T>* listener;

public:

	// Constructor and destructor
	ExecutionService();
	~ExecutionService();

	// Get data on our service given a key
	ExecutionOrder<T>& GetData(string _key);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(ExecutionOrder<T>& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<ExecutionOrder<T>>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<ExecutionOrder<T>>*>& GetListeners() const;

	// Get the listener of the service
	ExecutionToAlgoExecutionListener<T>* GetListener();

	// Execute an order on a market
	void ExecuteOrder(ExecutionOrder<T>& _executionOrder);

};

template<typename T>
ExecutionService<T>::ExecutionService()
{
	executionOrders = map<string, ExecutionOrder<T>>();
	listeners = vector<ServiceListener<ExecutionOrder<T>>*>();
	listener = new ExecutionToAlgoExecutionListener<T>(this);
}

template<typename T>
ExecutionService<T>::~ExecutionService() {}

template<typename T>
ExecutionOrder<T>& ExecutionService<T>::GetData(string _key)
{
	return executionOrders[_key];
}

template<typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T>& _data)
{
	executionOrders[_data.GetProduct().GetProductId()] = _data;
}

template<typename T>
void ExecutionService<T>::AddListener(ServiceListener<ExecutionOrder<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<ExecutionOrder<T>>*>& ExecutionService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
ExecutionToAlgoExecutionListener<T>* ExecutionService<T>::GetListener()
{
	return listener;
}

template<typename T>
void ExecutionService<T>::ExecuteOrder(ExecutionOrder<T>& _executionOrder)
{
	string _productId = _executionOrder.GetProduct().GetProductId();
	executionOrders[_productId] = _executionOrder;

	for (auto& l : listeners)
	{
		l->ProcessAdd(_executionOrder);
	}
}

/**
* Execution Service Listener subscribing data from Algo Execution Service to Execution Service.
* Type T is the product type.
*/
template<typename T>
class ExecutionToAlgoExecutionListener : public ServiceListener<AlgoExecution<T>>
{

private:

	ExecutionService<T>* service;

public:

	// Connector and Destructor
	ExecutionToAlgoExecutionListener(ExecutionService<T>* _service);
	~ExecutionToAlgoExecutionListener();

	// Listener callback to process an add event to the Service
	void ProcessAdd(AlgoExecution<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(AlgoExecution<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(AlgoExecution<T>& _data);

};

template<typename T>
ExecutionToAlgoExecutionListener<T>::ExecutionToAlgoExecutionListener(ExecutionService<T>* _service)
{
	service = _service;
}

template<typename T>
ExecutionToAlgoExecutionListener<T>::~ExecutionToAlgoExecutionListener() {}

template<typename T>
void ExecutionToAlgoExecutionListener<T>::ProcessAdd(AlgoExecution<T>& _data)
{
	ExecutionOrder<T>* _executionOrder = _data.GetExecutionOrder();
	service->OnMessage(*_executionOrder);
	service->ExecuteOrder(*_executionOrder);
}

template<typename T>
void ExecutionToAlgoExecutionListener<T>::ProcessRemove(AlgoExecution<T>& _data) {}

template<typename T>
void ExecutionToAlgoExecutionListener<T>::ProcessUpdate(AlgoExecution<T>& _data) {}

#endif
