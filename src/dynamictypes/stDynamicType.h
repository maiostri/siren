/**
 * @file
 *
 * This file implements the dynamic object types
 */
#ifndef __STDYNAMICTYPE_H
#define __STDYNAMICTYPE_H

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <math.h>

#include <arboretum/stBasicObjects.h>
#include <artemis/Artemis.hpp>

/**
 * Pure abstract class tDynamicType defines the Distance, Clone and other methods needed
 * for a dynamic type.
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicType {
protected:
		/**
		 * Pointer to the data vector
		 */
		double * Data;

		/**
		 * Size of the data vector
		 */
		stSize Size;


public:
		/**
		 * Clones the object.
		 */
		virtual tDynamicType *Clone() = 0;

		/**
		 * Gets the object serialized size
		 */
		virtual stSize GetSerializedSize() = 0;

		/**
		 * Returns the object serialization
		 */
		virtual const stByte * Serialize() = 0;

		/**
		 * Restores the serialized object.
		 */
		virtual void Unserialize(const stByte * data, stSize dataSize) = 0;

		/**
		 * Tests if one object is equal to another.
		 */
		virtual bool IsEqual(tDynamicType * obj) = 0;

		/**
		 * Returns a pointer to char with the data. Used to storage in database.
		 */
		char * convertDataToChar() {
				char *dataChar = new char(sizeof (this->Data));
				memcpy(dataChar, this->Data, sizeof (this->Data));
				return dataChar;
		};
		
		void setData(double *data) {
				this->Data = data;
		} 

};

/**
 * Normalized histogram
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicHistogram : public tDynamicType {
public:

		/**
		 * Constructor.
		 */
		tDynamicHistogram() {
				Size = 0;
				Data = NULL;
		}

		/**
		 * Constructor.
		 */
		tDynamicHistogram(stSize n, const double *data = NULL) {
				Size = n;
				Data = new double[Size];
				if (data != NULL)
						for (stSize i = 0; i < n; i++) Data[i] = data[i];
		}

		/**
		 * Destructor.
		 */
		virtual ~tDynamicHistogram() {
				delete[] Data;
		}

		/**
		 * Clones the object.
		 */
		virtual tDynamicType *Clone() {
				tDynamicHistogram *clone = new tDynamicHistogram(this->Size, this->Data);
				return (tDynamicType *) clone;
		}

		/**
		 * Gets the object serialized size
		 */
		virtual stSize GetSerializedSize() {
				return sizeof (double) * Size;
		}

		/**
		 * Returns the object serialization
		 */
		virtual const stByte * Serialize() {
				return (unsigned char *) Data;
		}

		/**
		 * Restores the serialized object.
		 */
		virtual void Unserialize(const stByte * data, stSize dataSize) {
				// Resize data
				Size = dataSize / sizeof (double);
				//Size = dataSize / sizeof(stBasicArrayObject<double>);
				if (Data != NULL)
						delete Data;
				Data = new double[Size];
				memcpy(Data, data, dataSize);
		}

		/**
		 * Tests if one object is equal to another.
		 */
		virtual bool IsEqual(tDynamicType * obj) {
				bool equal;
				bool stop;
				stSize i;

				if (Size != ((tDynamicHistogram *) obj)->Size) return false;

				equal = true;
				i = 0;
				stop = (i >= Size);
				while (!stop) {
						if (Data[i] != ((tDynamicHistogram *) obj)->Data[i]) {
								//((tDynamicHistogram *)obj)->(*Data[i].GetData())) {
								stop = true;
								equal = false;
						} else {
								i++;
								stop = (i >= Size);
						}//end if
				}//end while
				return equal;
		}

		/**
		 * Extract the histogram from a bitmap
		 */
		void ExtractHistogram(Image* src) {
				// width and heightS
				NormalizedHistogramExtractor<stBasicArrayObject<double> > *extractor =
								new NormalizedHistogramExtractor<stBasicArrayObject<double> >();

				try {
						stBasicArrayObject<double> *Se = new stBasicArrayObject<double>(256);
						extractor->generateSignature(*src, *Se);
						if (Data != NULL)
								delete Data;
						Data = new double[Se->GetSize()];
						for (unsigned int i = 0; i < Se->GetSize(); i++) Data[i] = *(double*) Se->Get(i);
						delete Se;
				} catch (NotAvaliableException* ne) {
						printf("Erro no descritor: Erro nos recursos do extrator\n");
						delete extractor;
						delete Data;
				} catch (artemis::FullHeapException* fe) {
						printf("Erro no descritor: Erro na alocação de memória de recursos\n");
						delete extractor;
				}
		}

		/**
		 * Returns the size of the histogram
		 */
		stSize GetSize() {
				return Size;
		}

		/**
		 * Returns a value of the histogram
		 */
		double GetData(stSize index) {
				if (index < Size)
						return Data[index];
				else
						return -1;
		}
private:

		/**
		 * Sizes or resizes the histogram
		 */
		void SetSize(stSize n) {
				Size = n;
				if (Data != NULL)
						delete Data;
				Data = new double[Size];
				for (stSize i = 0; i < n; i++)
						Data[i] = 0;
		}

};

/**
 * Metric histogram
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicMetricHistogram : public tDynamicType {
public:

		/**
		 * Constructor.
		 */
		tDynamicMetricHistogram() {
		}

		/**
		 * Destructor.
		 */
		virtual ~tDynamicMetricHistogram() {
		}

		/**
		 * Clones the object.
		 */
		virtual tDynamicType *Clone() {
				tDynamicType *clone = new tDynamicMetricHistogram();
				return clone;
		}

		/**
		 * Gets the object serialized size
		 */
		virtual stSize GetSerializedSize() {
				return 0;
		}

		/**
		 * Returns the object serialization
		 */
		virtual const stByte * Serialize() {
				return 0;
		}

		/**
		 * Restores the serialized object.
		 */
		virtual void Unserialize(const stByte * data, stSize dataSize) {
		}

		/**
		 * Tests if one object is equal to another.
		 */
		virtual bool IsEqual(tDynamicType * obj) {
				return 0;
		}

		/**
		 * Extract the metric histogram from a jpeg file
		 */
		void ExtractMetricHistogram(const Image *img) {

		}

		/**
		 * Extract the metric histogram from a bitmap
		 */


private:
};

/**
 * Particulate Object
 * @author Humberto Razente
 * @author Maria Camila Barioni
 */
class tDynamicParticulate : public tDynamicType {
public:

		/**
		 * Constructor.
		 */
		tDynamicParticulate() {
				Size = 0;
				Data = NULL;
		}

		/**
		 * Constructor.
		 */
		tDynamicParticulate(stSize n, const double * data = NULL) {
				Size = n;
				Data = new double[n];
				if (data != NULL) {
						for (stSize i = 0; i < n; i++)
								Data[i] = data[i];
				}
		}

		/**
		 * Destructor.
		 */
		virtual ~tDynamicParticulate() {
				delete Data;
		}

		/**
		 * Clones the object.
		 */
		virtual tDynamicType *Clone() {
				tDynamicParticulate *clone = new tDynamicParticulate(this->Size, this->Data);
				return (tDynamicType *) clone;
		}

		/**
		 * Gets the object serialized size
		 */
		virtual stSize GetSerializedSize() {
				return sizeof (double) * Size;
		}

		/**
		 * Returns the object serialization
		 */
		virtual const stByte * Serialize() {
				return (unsigned char *) Data;
		}

		/**
		 * Restores the serialized object.
		 */
		virtual void Unserialize(const stByte * data, stSize dataSize) {
				// Resize data
				Size = dataSize / sizeof (double);
				if (Data != NULL)
						delete Data;
				Data = new double[Size];
				memcpy(Data, data, dataSize);
		}

		/**
		 * Tests if one object is equal to another.
		 */
		virtual bool IsEqual(tDynamicType * obj) {
				bool equal;
				bool stop;
				stSize i;

				if (Size != ((tDynamicParticulate *) obj)->Size) return false;

				equal = true;
				i = 0;
				stop = (i >= Size);
				while (!stop) {
						if (Data[i] != ((tDynamicParticulate *) obj)->Data[i]) {
								stop = true;
								equal = false;
						} else {
								i++;
								stop = (i >= Size);
						}//end if
				}//end while
				return equal;
		}

		/**
		 * Sizes or resizes the object
		 */
		void SetSize(stSize n) {
				Size = n;
				if (Data != NULL)
						delete Data;
				Data = new double[n];
				for (stSize i = 0; i < n; i++)
						Data[i] = 0;
		}

		/**
		 * Returns the size of the histogram
		 */
		stSize GetSize() {
				return Size;
		}

		/**
		 * Returns an attribute value
		 */
		double GetData(stSize index) {
				if (index < Size)
						return Data[index];
				else
						return -1;
		}

		/**
		 * Sets the attributes values
		 */
		void SetData(stSize n, const double * data = NULL) {
				Size = n;
				Data = new double[n];
				if (data != NULL) {
						for (stSize i = 0; i < n; i++)
								Data[i] = data[i];
				}
		}

		/**
		 * Sets the attributes values
		 */
		void SetParticle(stSize position, double data) {
				if (position < Size)
						Data[position] = data;
		}
};

#endif
