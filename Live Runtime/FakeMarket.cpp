#include "functions.h"

#define MIN_PRICE 0.1
#define MAX_PRICE 62000
#define VOLATILITY 8
#define MAX_MARKET_HISTORY 1000000

float oldPrice = 5.0;
std::vector<float> marketData;

void UpdateFakeMarket() {
	float changePercent = 2*VOLATILITY*(getRandomFloat() - 0.5);
	float changeAmount = oldPrice * changePercent/100;
    float newPrice = oldPrice + changeAmount;

    if (newPrice < MIN_PRICE)
        newPrice += abs(changeAmount) * 2;
    else if (newPrice > MAX_PRICE)
        newPrice -= abs(changeAmount) * 2;
    
	if (marketData.size() == 0)
		marketData.push_back(oldPrice);
	marketData.push_back(newPrice);
	if (marketData.size() > MAX_MARKET_HISTORY) {
		marketData.erase(marketData.begin());
	}
	
	oldPrice = newPrice;
}

float GetFakeMarketPrice() { return oldPrice; }
std::vector<float> GetFakeMarketHistory() { return marketData; }

float GetAndUpdateFakeMarketPrice() {
	UpdateFakeMarket();
	return GetFakeMarketPrice();
}