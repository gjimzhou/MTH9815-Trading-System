#include <iostream>
#include <string>
#include <map>

#include "soa.hpp"
#include "products.hpp"
#include "algoexecutionservice.hpp"
#include "algostreamingservice.hpp"
#include "executionservice.hpp"
#include "guiservice.hpp"
#include "historicaldataservice.hpp"
#include "inquiryservice.hpp"
#include "marketdataservice.hpp"
#include "positionservice.hpp"
#include "pricingservice.hpp"
#include "riskservice.hpp"
#include "streamingservice.hpp"
#include "tradebookingservice.hpp"

using namespace std;

int main()
{
	cout << TimeStamp() << "Program Starting..." << endl;
	cout << TimeStamp() << "Program Started." << endl;

	cout << TimeStamp() << "Services Initializing..." << endl;
	PricingService<Bond> pricingService;
	TradeBookingService<Bond> tradeBookingService;
	PositionService<Bond> positionService;
	RiskService<Bond> riskService;
	MarketDataService<Bond> marketDataService;
	AlgoExecutionService<Bond> algoExecutionService;
	AlgoStreamingService<Bond> algoStreamingService;
	GUIService<Bond> guiService;
	ExecutionService<Bond> executionService;
	StreamingService<Bond> streamingService;
	InquiryService<Bond> inquiryService;
	HistoricalDataService<Position<Bond>> historicalPositionService(POSITION);
	HistoricalDataService<PV01<Bond>> historicalRiskService(RISK);
	HistoricalDataService<ExecutionOrder<Bond>> historicalExecutionService(EXECUTION);
	HistoricalDataService<PriceStream<Bond>> historicalStreamingService(STREAMING);
	HistoricalDataService<Inquiry<Bond>> historicalInquiryService(INQUIRY);
	cout << TimeStamp() << "Services Initialized." << endl;

	cout << TimeStamp() << "Services Linking..." << endl;
	pricingService.AddListener(algoStreamingService.GetListener());
	pricingService.AddListener(guiService.GetListener());
	algoStreamingService.AddListener(streamingService.GetListener());
	streamingService.AddListener(historicalStreamingService.GetListener());
	marketDataService.AddListener(algoExecutionService.GetListener());
	algoExecutionService.AddListener(executionService.GetListener());
	executionService.AddListener(tradeBookingService.GetListener());
	executionService.AddListener(historicalExecutionService.GetListener());
	tradeBookingService.AddListener(positionService.GetListener());
	positionService.AddListener(riskService.GetListener());
	positionService.AddListener(historicalPositionService.GetListener());
	riskService.AddListener(historicalRiskService.GetListener());
	inquiryService.AddListener(historicalInquiryService.GetListener());
	cout << TimeStamp() << "Services Linked." << endl;

	cout << TimeStamp() << "Price Data Processing..." << endl;
	ifstream priceData("prices.txt");
	pricingService.GetConnector()->Subscribe(priceData);
	cout << TimeStamp() << "Price Data Processed." << endl;

	cout << TimeStamp() << "Trade Data Processing..." << endl;
	ifstream tradeData("trades.txt");
	tradeBookingService.GetConnector()->Subscribe(tradeData);
	cout << TimeStamp() << "Trade Data Processed." << endl;

	cout << TimeStamp() << "Market Data Processing..." << endl;
	ifstream marketData("marketdata.txt");
	marketDataService.GetConnector()->Subscribe(marketData);
	cout << TimeStamp() << "Market Data Processed." << endl;

	cout << TimeStamp() << "Inquiry Data Processing..." << endl;
	ifstream inquiryData("inquiries.txt");
	inquiryService.GetConnector()->Subscribe(inquiryData);
	cout << TimeStamp() << "Inquiry Data Processed." << endl;

	cout << TimeStamp() << "Program Ending..." << endl;
	cout << TimeStamp() << "Program Ended." << endl;
	system("pause");
	return 0;
}

