/**
* algoexecutionservice.hpp
* Defines the data types and Service for algo executions.
*
* @author Breman Thuraisingham
* @coauthor Junliang Jimmy Zhou
*/
#ifndef ALGO_EXECUTION_SERVICE_HPP
#define ALGO_EXECUTION_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include "marketdataservice.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
* An execution order that can be placed on an exchange.
* Type T is the product type.
*/
template<typename T>
class ExecutionOrder
{

public:

	// ctor for an order
	ExecutionOrder() = default;
	ExecutionOrder(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

	// Get the product
	const T& GetProduct() const;

	// Get the pricing side
	PricingSide GetPricingSide() const;

	// Get the order ID
	const string& GetOrderId() const;

	// Get the order type on this order
	OrderType GetOrderType() const;

	// Get the price on this order
	double GetPrice() const;

	// Get the visible quantity on this order
	long GetVisibleQuantity() const;

	// Get the hidden quantity
	long GetHiddenQuantity() const;

	// Get the parent order ID
	const string& GetParentOrderId() const;

	// Is child order?
	bool IsChildOrder() const;

	// Change attributes to strings
	vector<string> ToStrings() const;

private:
	T product;
	PricingSide side;
	string orderId;
	OrderType orderType;
	double price;
	long visibleQuantity;
	long hiddenQuantity;
	string parentOrderId;
	bool isChildOrder;

};

template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
	product(_product)
{
	side = _side;
	orderId = _orderId;
	orderType = _orderType;
	price = _price;
	visibleQuantity = _visibleQuantity;
	hiddenQuantity = _hiddenQuantity;
	parentOrderId = _parentOrderId;
	isChildOrder = _isChildOrder;
}

template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
	return product;
}

template<typename T>
PricingSide ExecutionOrder<T>::GetPricingSide() const
{
	return side;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
	return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
	return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
	return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
	return visibleQuantity;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
	return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
	return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
	return isChildOrder;
}

template<typename T>
vector<string> ExecutionOrder<T>::ToStrings() const
{
	string _product = product.GetProductId();
	string _side;
	switch (side)
	{
	case BID:
		_side = "BID";
		break;
	case OFFER:
		_side = "OFFER";
		break;
	}
	string _orderId = orderId;
	string _orderType;
	switch (orderType)
	{
	case FOK:
		_orderType = "FOK";
		break;
	case IOC:
		_orderType = "IOC";
		break;
	case MARKET:
		_orderType = "MARKET";
		break;
	case LIMIT:
		_orderType = "LIMIT";
		break;
	case STOP:
		_orderType = "STOP";
		break;
	}
	string _price = ConvertPrice(price);
	string _visibleQuantity = to_string(visibleQuantity);
	string _hiddenQuantity = to_string(hiddenQuantity);
	string _parentOrderId = parentOrderId;
	string _isChildOrder;
	switch (isChildOrder)
	{
	case true:
		_isChildOrder = "YES";
		break;
	case false:
		_isChildOrder = "NO";
		break;
	}

	vector<string> _strings;
	_strings.push_back(_product);
	_strings.push_back(_side);
	_strings.push_back(_orderId);
	_strings.push_back(_orderType);
	_strings.push_back(_price);
	_strings.push_back(_visibleQuantity);
	_strings.push_back(_hiddenQuantity);
	_strings.push_back(_parentOrderId);
	_strings.push_back(_isChildOrder);
	return _strings;
}

/**
* An algo execution that process algo execution.
* Type T is the product type.
*/
template<typename T>
class AlgoExecution
{

public:

	// ctor for an order
	AlgoExecution() = default;
	AlgoExecution(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

	// Get the order
	ExecutionOrder<T>* GetExecutionOrder() const;

private:
	ExecutionOrder<T>* executionOrder;

};

template<typename T>
AlgoExecution<T>::AlgoExecution(const T& _product, PricingSide _side, string _orderId, OrderType _orderType, double _price, long _visibleQuantity, long _hiddenQuantity, string _parentOrderId, bool _isChildOrder)
{
	executionOrder = new ExecutionOrder<T>(_product, _side, _orderId, _orderType, _price, _visibleQuantity, _hiddenQuantity, _parentOrderId, _isChildOrder);
}

template<typename T>
ExecutionOrder<T>* AlgoExecution<T>::GetExecutionOrder() const
{
	return executionOrder;
}

/**
* Pre-declearations to avoid errors.
*/
template<typename T>
class AlgoExecutionToMarketDataListener;

/**
* Service for algo executing orders on an exchange.
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class AlgoExecutionService : public Service<string, AlgoExecution<T>>
{

private:

	map<string, AlgoExecution<T>> algoExecutions;
	vector<ServiceListener<AlgoExecution<T>>*> listeners;
	AlgoExecutionToMarketDataListener<T>* listener;
	double spread;
	long count;

public:

	// Constructor and destructor
	AlgoExecutionService();
	~AlgoExecutionService();

	// Get data on our service given a key
	AlgoExecution<T>& GetData(string _key);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(AlgoExecution<T>& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<AlgoExecution<T>>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<AlgoExecution<T>>*>& GetListeners() const;

	// Get the listener of the service
	AlgoExecutionToMarketDataListener<T>* GetListener();

	// Execute an order on a market
	void AlgoExecuteOrder(OrderBook<T>& _orderBook);

};

template<typename T>
AlgoExecutionService<T>::AlgoExecutionService()
{
	algoExecutions = map<string, AlgoExecution<T>>();
	listeners = vector<ServiceListener<AlgoExecution<T>>*>();
	listener = new AlgoExecutionToMarketDataListener<T>(this);
	spread = 1.0 / 128.0;
	count = 0;
}

template<typename T>
AlgoExecutionService<T>::~AlgoExecutionService() {}

template<typename T>
AlgoExecution<T>& AlgoExecutionService<T>::GetData(string _key)
{
	return algoExecutions[_key];
}

template<typename T>
void AlgoExecutionService<T>::OnMessage(AlgoExecution<T>& _data)
{
	algoExecutions[_data.GetExecutionOrder()->GetProduct().GetProductId()] = _data;
}

template<typename T>
void AlgoExecutionService<T>::AddListener(ServiceListener<AlgoExecution<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<AlgoExecution<T>>*>& AlgoExecutionService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
AlgoExecutionToMarketDataListener<T>* AlgoExecutionService<T>::GetListener()
{
	return listener;
}

template<typename T>
void AlgoExecutionService<T>::AlgoExecuteOrder(OrderBook<T>& _orderBook)
{
	T _product = _orderBook.GetProduct();
	string _productId = _product.GetProductId();
	PricingSide _side;
	string _orderId = GenerateId();
	double _price;
	long _quantity;

	BidOffer _bidOffer = _orderBook.GetBidOffer();
	Order _bidOrder = _bidOffer.GetBidOrder();
	double _bidPrice = _bidOrder.GetPrice();
	long _bidQuantity = _bidOrder.GetQuantity();
	Order _offerOrder = _bidOffer.GetOfferOrder();
	double _offerPrice = _offerOrder.GetPrice();
	long _offerQuantity = _offerOrder.GetQuantity();

	if (_offerPrice - _bidPrice <= spread)
	{
		switch (count % 2)
		{
		case 0:
			_price = _bidPrice;
			_quantity = _bidQuantity;
			_side = BID;
			break;
		case 1:
			_price = _offerPrice; 
			_quantity = _offerQuantity;
			_side = OFFER;
			break;
		}
		count++;
		AlgoExecution<T> _algoExecution(_product, _side, _orderId, MARKET, _price, _quantity, 0, "", false);
		algoExecutions[_productId] = _algoExecution;

		for (auto& l : listeners)
		{
			l->ProcessAdd(_algoExecution);
		}
	}
}

/**
* Algo Execution Service Listener subscribing data from Market Data Service to Algo Execution Service.
* Type T is the product type.
*/
template<typename T>
class AlgoExecutionToMarketDataListener : public ServiceListener<OrderBook<T>>
{

private:

	AlgoExecutionService<T>* service;

public:

	// Connector and Destructor
	AlgoExecutionToMarketDataListener(AlgoExecutionService<T>* _service);
	~AlgoExecutionToMarketDataListener();

	// Listener callback to process an add event to the Service
	void ProcessAdd(OrderBook<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(OrderBook<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(OrderBook<T>& _data);

};

template<typename T>
AlgoExecutionToMarketDataListener<T>::AlgoExecutionToMarketDataListener(AlgoExecutionService<T>* _service)
{
	service = _service;
}

template<typename T>
AlgoExecutionToMarketDataListener<T>::~AlgoExecutionToMarketDataListener() {}

template<typename T>
void AlgoExecutionToMarketDataListener<T>::ProcessAdd(OrderBook<T>& _data)
{
	service->AlgoExecuteOrder(_data);
}

template<typename T>
void AlgoExecutionToMarketDataListener<T>::ProcessRemove(OrderBook<T>& _data) {}

template<typename T>
void AlgoExecutionToMarketDataListener<T>::ProcessUpdate(OrderBook<T>& _data) {}

#endif
