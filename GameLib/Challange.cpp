#include "Challange.h"

Challange::Challange() : firstValue(0), secondValue(0){}

Challange::Challange(unsigned short _firstValue, unsigned short _secondValue) : firstValue(_firstValue), secondValue(_secondValue) {}

Challange::~Challange() {}

unsigned short Challange::GetFirstValue()
{
	return firstValue;
}

unsigned short Challange::GetSecondValue()
{
	return secondValue;
}

unsigned short Challange::GetResult()
{
	return firstValue + secondValue;
}
