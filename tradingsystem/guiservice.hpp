/**
* guiservice.hpp
* Defines the data types and Service for GUI output.
*
* @author Breman Thuraisingham
* @coauthor Junliang Jimmy Zhou
*/
#ifndef GUI_SERVICE_HPP
#define GUI_SERVICE_HPP

#include "soa.hpp"
#include "pricingservice.hpp"

/**
* Pre-declearations to avoid errors.
*/
template<typename T>
class GUIConnector;
template<typename T>
class GUIToPricingListener;

/**
* Service for outputing GUI with a certain throttle.
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class GUIService : Service<string, Price<T>>
{

private:

	map<string, Price<T>> guis;
	vector<ServiceListener<Price<T>>*> listeners;
	GUIConnector<T>* connector;
	ServiceListener<Price<T>>* listener;
	int throttle;
	long millisec;

public:

	// Constructor and destructor
	GUIService();
	~GUIService();

	// Get data on our service given a key
	Price<T>& GetData(string _key);

	// The callback that a Connector should invoke for any new or updated data
	void OnMessage(Price<T>& _data);

	// Add a listener to the Service for callbacks on add, remove, and update events for data to the Service
	void AddListener(ServiceListener<Price<T>>* _listener);

	// Get all listeners on the Service
	const vector<ServiceListener<Price<T>>*>& GetListeners() const;

	// Get the connector of the service
	GUIConnector<T>* GetConnector();

	// Get the listener of the service
	ServiceListener<Price<T>>* GetListener();

	// Get the throttle of the service
	int GetThrottle() const;

	// Get the millisec of the service
	long GetMillisec() const;

	// Set the millisec of the service
	void SetMillisec(long _millisec);

};

template<typename T>
GUIService<T>::GUIService()
{
	guis = map<string, Price<T>>();
	listeners = vector<ServiceListener<Price<T>>*>();
	connector = new GUIConnector<T>(this);
	listener = new GUIToPricingListener<T>(this);
	throttle = 300;
	millisec = 0;
}

template<typename T>
GUIService<T>::~GUIService() {}

template<typename T>
Price<T>& GUIService<T>::GetData(string _key)
{
	return guis[_key];
}

template<typename T>
void GUIService<T>::OnMessage(Price<T>& _data)
{
	guis[_data.GetProduct().GetProductId()] = _data;
	connector->Publish(_data);
}

template<typename T>
void GUIService<T>::AddListener(ServiceListener<Price<T>>* _listener)
{
	listeners.push_back(_listener);
}

template<typename T>
const vector<ServiceListener<Price<T>>*>& GUIService<T>::GetListeners() const
{
	return listeners;
}

template<typename T>
GUIConnector<T>* GUIService<T>::GetConnector()
{
	return connector;
}

template<typename T>
ServiceListener<Price<T>>* GUIService<T>::GetListener()
{
	return listener;
}

template<typename T>
int GUIService<T>::GetThrottle() const
{
	return throttle;
}

template<typename T>
long GUIService<T>::GetMillisec() const
{
	return millisec;
}

template<typename T>
void GUIService<T>::SetMillisec(long _millisec)
{
	millisec = _millisec;
}


/**
* GUI Connector publishing data from GUI Service.
* Type T is the product type.
*/
template<typename T>
class GUIConnector : public Connector<Price<T>>
{

private:

	GUIService<T>* service;

public:

	// Connector and Destructor
	GUIConnector(GUIService<T>* _service);
	~GUIConnector();

	// Publish data to the Connector
	void Publish(Price<T>& _data);

	// Subscribe data from the Connector
	void Subscribe(ifstream& _data);

};

template<typename T>
GUIConnector<T>::GUIConnector(GUIService<T>* _service)
{
	service = _service;
}

template<typename T>
GUIConnector<T>::~GUIConnector() {}

template<typename T>
void GUIConnector<T>::Publish(Price<T>& _data)
{
	int _throttle = service->GetThrottle();
	long _millisec = service->GetMillisec();
	long _millisecNow = GetMillisecond();
	while (_millisecNow < _millisec) _millisecNow += 1000;
	if (_millisecNow - _millisec >= _throttle)
	{
		service->SetMillisec(_millisecNow);
		ofstream _file;
		_file.open("gui.txt", ios::app);

		_file << TimeStamp() << ",";
		vector<string> _strings = _data.ToStrings();
		for (auto& s : _strings)
		{
			_file << s << ",";
		}
		_file << endl;
	}
}

template<typename T>
void GUIConnector<T>::Subscribe(ifstream& _data) {}

/**
* GUI Service Listener subscribing data to GUI Data.
* Type T is the product type.
*/
template<typename T>
class GUIToPricingListener : public ServiceListener<Price<T>>
{

private:

	GUIService<T>* service;

public:

	// Connector and Destructor
	GUIToPricingListener(GUIService<T>* _service);
	~GUIToPricingListener();

	// Listener callback to process an add event to the Service
	void ProcessAdd(Price<T>& _data);

	// Listener callback to process a remove event to the Service
	void ProcessRemove(Price<T>& _data);

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Price<T>& _data);

};

template<typename T>
GUIToPricingListener<T>::GUIToPricingListener(GUIService<T>* _service)
{
	service = _service;
}

template<typename T>
GUIToPricingListener<T>::~GUIToPricingListener() {}

template<typename T>
void GUIToPricingListener<T>::ProcessAdd(Price<T>& _data)
{
	service->OnMessage(_data);
}

template<typename T>
void GUIToPricingListener<T>::ProcessRemove(Price<T>& _data) {}

template<typename T>
void GUIToPricingListener<T>::ProcessUpdate(Price<T>& _data) {}

#endif
