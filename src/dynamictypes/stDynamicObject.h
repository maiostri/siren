/**
 * @file
 *
 * This file implements the dynamic object
 */
#ifndef __STDYNAMICBJECT_H
#define __STDYNAMICBJECT_H

#include <vector>

#include <boost/lexical_cast.hpp>

#include <arboretum/stTypes.h>
#include <arboretum/stObject.h>
#include <arboretum/stException.h>

#include "stDynamicType.h"
#include "stDynamicHistogram.h"
#include "stDynamicMetricHistogram.h"
#include "stDynamicParticulate.h"
#include "stDynamicTexture.h"
#include "stDynamicZernike.h"

using namespace std;
using namespace boost;

/**
 * This class implements the dynamic object.
 * In order to add new characteristics to this object,
 * the methods Clone() and IsEqual() must be altered.
 *
 * @author Humberto Razente
 * @author Maria Camila Nardini Barioni
 */
class tDynamicObject: public stObject {
private:
	/**
	 * Pointer to a histogram.
	 */
	tDynamicHistogram *histogram;

	/**
	 * Pointer to a metric histogram.
	 */
	tDynamicMetricHistogram *metrichistogram;

	/**
	 * Pointer to texture feature.
	 */
	tDynamicTexture *texture;

	tDynamicTexture *sample;

	/**
	 * Pointer to particulate feature.
	 */
	tDynamicParticulate *particulate;

	/**
	 * Pointer to texture feature.
	 */
	tDynamicZernike *zernike;

	/**
	 * Pointer to a sound texture stft.
	 */
	tDynamicHistogram *soundtexturestft;

	/**
	 * Pointer to a sound texture stft.
	 */
	tDynamicHistogram *soundtexturemfcc;

	/**
	 * Object identification.
	 */
	long OID;

	/**
	 * Multimedia file source
	 */
	std::string FileName;

public:

	/**
	 * Constructor.
	 */
	tDynamicObject() {
		OID = 0;
		histogram = NULL;
		metrichistogram = NULL;
		texture = NULL;
		particulate = NULL;
		zernike = NULL;
		FileName = "";
	} //end tDynamicObject

	/**
	 * Destructor.
	 */
	virtual ~tDynamicObject() {
		if (histogram != NULL) {
			delete histogram;
		}
		if (metrichistogram != NULL) {
			delete metrichistogram;
		}
		if (texture != NULL) {
			delete texture;
		}
		if (particulate != NULL) {
			delete particulate;
		}
		if (zernike != NULL) {
			delete zernike;
		}

	} //end ~tDynamicObject

	/**
	 * Sets multimedia file source name
	 */
	void SetFileName(string fn) {
		FileName = fn;
	}

	/**
	 * Gets multimedia file source name
	 */
	string GetFileName() const {
		return FileName;
	}

	virtual void *
	Get(stCount idx) {
		return NULL;
	}

	virtual stSize GetSize() {
		return 0;
	}

	/**
	 * Returns the tDynamicType feature
	 */
	tDynamicType *
	GetExtraction(const string& extractor) {
		// extractor == histogram
		if (extractor.compare("histogramext") == 0) {
			//if (parameter.compare("histogram") == 0)
			return (tDynamicType *) GetHistogram();
		} // extractor == metric histogram
		else if (extractor.compare("metrichistogramext") == 0) {
			//if (parameter.compare("metrichistogram") == 0)
			return (tDynamicType *) GetMetricHistogram();
		} // extractor == texture
		else if (extractor.compare("textureext") == 0) {
			//if (parameter.compare("texture") == 0)
			return (tDynamicType *) GetTexture();
		} // extractor == zernike
		else if (extractor.compare("zernikeext") == 0) {
			//if (parameter.compare("zernike") == 0)
			return (tDynamicZernike *) GetZernike();
		}
		return NULL;
	}

	/**
	 * This function receives the data of the extractor as a char
	 * and copy it to a double which will be attributed in the tDynamicObject.
	 * @param data
	 *				The extractor data.
	 */
	double *
	getDataDynamicType(char *data) {
		double *dataDouble = new double();
		memcpy(dataDouble, data, sizeof(data));
		return dataDouble;
	}

	/**
	 * Here we set the DynamicType of the DynamicObject based on the parameter.
	 * @param parameter
	 * @param data
	 */
	void setData(string parameter, char *data) {
		tDynamicType *tDynamicTypeObject;
		double *dataDouble = getDataDynamicType(data);

		if (parameter.compare("histogram") == 0) {
			tDynamicTypeObject = new tDynamicHistogram();
			tDynamicTypeObject->setData(dataDouble);
			this->SetHistogram((tDynamicHistogram *) tDynamicTypeObject);
		} else if (parameter.compare("metrichistogram") == 0) {
			tDynamicTypeObject = new tDynamicMetricHistogram();
			tDynamicTypeObject->setData(dataDouble);
			this->SetMetricHistogram((tDynamicMetricHistogram *) tDynamicTypeObject);
			// @TODO - That is important.
			// The specifications of the texture extractor are different.
			// Let's wait until the new extractor is implemented.
		} else if (parameter.compare("texture") == 0) {
			//tDynamicTypeObject = new tDynamicTexture();
			//tDynamicTypeObject->setData(dataDouble);
			//this->SetTexture((tDynamicTexture *) tDynamicTypeObject);
		} else if (parameter.compare("zernike") == 0) {
			tDynamicTypeObject = new tDynamicZernike();
			tDynamicTypeObject->setData(dataDouble);
			this->SetZernike((tDynamicZernike *) tDynamicTypeObject);
		}
	}

	/**
	 * Extracts the features of an image
	 */
	void Extract(const std::string& extractor, const std::string& parameter, Image* src) {
		// extractor == histogram
		if (extractor.compare("histogramext") == 0) {
			if (parameter.compare("histogram") == 0) {
				tDynamicHistogram *h = new tDynamicHistogram();
				h->ExtractHistogram(src);
				if (this->GetHistogram() == NULL)
					this->SetHistogram(h);
			}
		}            // extractor == metric histogram
		else if (extractor.compare("metrichistogramext") == 0) {
			if (parameter.compare("metrichistogram") == 0) {
				tDynamicMetricHistogram *m = new tDynamicMetricHistogram();
				m->ExtractMetricHistogram(src);
				if (this->GetMetricHistogram() == NULL)
					this->SetMetricHistogram(m);
			}
		}            // extractor == texture
		else if (extractor.compare("textureext") == 0) {
			if (parameter.compare("texture") == 0) {
				tDynamicTexture *t = new tDynamicTexture();
				t->ExtractTexture(src);
				if (this->GetTexture() == NULL)
					this->SetTexture(t);
			}
		}            // @TODO This extractor isn't implemented yet.
		else if (extractor.compare("zernikeext") == 0) {
			if (parameter.compare("zernike") == 0) {
			}
		}
	}

	/**
	 * Sets the histogram.
	 */
	void SetHistogram(tDynamicHistogram *h) {
		this->histogram = h;
	}

	/**
	 * Sets the histogram.
	 */
	void SetHistogram(stSize n, double* data = NULL) {
		if (histogram == NULL) {
			histogram = new tDynamicHistogram(n, data);
		} else {
			delete histogram;
			histogram = new tDynamicHistogram(n, data);
		}
	}

	/**
	 * Gets the histogram.
	 */
	tDynamicHistogram *
	GetHistogram() {
		return histogram;
	}

	/**
	 * Sets the metric histogram.
	 */
	void SetMetricHistogram(tDynamicMetricHistogram *m) {
		this->metrichistogram = m;
	}

	/**
	 * Gets the metric histogram.
	 */
	tDynamicMetricHistogram *
	GetMetricHistogram() {
		return metrichistogram;
	}

	/**
	 * Sets the texture.
	 */
	void SetTexture(tDynamicTexture *m) {
		this->texture = m;
	}

	/**
	 * Gets the texture.
	 */
	tDynamicTexture *
	GetTexture() {
		return texture;
	}

	/**
	 * Sets the particulate.
	 */
	void SetParticulate(tDynamicParticulate *p) {
		particulate = (tDynamicParticulate *) p->Clone();
	}

	/**
	 * Gets the particulate.
	 */
	tDynamicParticulate *
	GetParticulate() {
		return particulate;
	}

	/**
	 * Sets the zernike.
	 */
	void SetZernike(tDynamicZernike *m) {
		this->zernike = m;
	}

	/**
	 * Gets the zernike.
	 */
	tDynamicZernike *
	GetZernike() {
		return zernike;
	}

	/**
	 * Clones the object.
	 */
	virtual tDynamicObject *
	Clone() {
		// instantiate the new object
		tDynamicObject * tmp = new tDynamicObject();

		// set identification
		tmp->SetOID(this->GetOID());
		tmp->SetFileName(this->GetFileName());

		// set the histogram
		if (this->GetHistogram() != NULL) {
			tmp->SetHistogram(this->GetHistogram()->Clone());
		}

		// set the metric histogram
		if (this->GetMetricHistogram() != NULL) {
			tmp->SetMetricHistogram(this->GetMetricHistogram()->Clone());
		}

		// set the texture
		if (this->GetTexture() != NULL) {
			tmp->SetTexture(this->GetTexture()->Clone());
		}

		// set the particulate
		if (this->GetParticulate() != NULL) {
			tmp->SetParticulate(this->GetParticulate()->Clone());
		}

		// set the zernike
		if (this->GetZernike() != NULL) {
			tmp->SetZernike(this->GetZernike()->Clone());
		}

		// return new object
		return tmp;
	}            //end Clone

	/**
	 * Tests if one object is equal to another.
	 */
	virtual bool IsEqual(stObject * obj) {
		bool histogramIsEqual = true;
		bool metrichistogramIsEqual = true;
		bool textureIsEqual = true;
		bool particulateIsEqual = true;
		bool zernikeIsEqual = true;
		bool soundtexturestftIsEqual = true;
		bool soundtexturemfccIsEqual = true;

		// test 1: tests if each data type is equal
		if (this->GetHistogram() != NULL)
			histogramIsEqual = this->GetHistogram()->IsEqual(((tDynamicObject *) obj)->GetHistogram());
		if (this->GetMetricHistogram() != NULL)
			metrichistogramIsEqual = this->GetMetricHistogram()->IsEqual(
					((tDynamicObject *) obj)->GetMetricHistogram());
		if (this->GetTexture() != NULL)
			//				textureIsEqual = this->GetTexture()->IsEqual(((tDynamicObject *)obj)->GetTexture());
			if (this->GetParticulate() != NULL)
				particulateIsEqual = this->GetParticulate()->IsEqual(((tDynamicObject *) obj)->GetParticulate());
		if (this->GetZernike() != NULL)
			zernikeIsEqual = this->GetZernike()->IsEqual(((tDynamicObject *) obj)->GetZernike());

		// test 2: tests if a data type not present in this object is present on the other
		if ((this->GetHistogram() == NULL) && (((tDynamicObject *) obj)->GetHistogram() != NULL))
			histogramIsEqual = false;
		if ((this->GetMetricHistogram() == NULL) && (((tDynamicObject *) obj)->GetMetricHistogram() != NULL))
			metrichistogramIsEqual = false;
		if ((this->GetTexture() == NULL) && (((tDynamicObject *) obj)->GetTexture() != NULL))
			textureIsEqual = false;
		if ((this->GetParticulate() == NULL) && (((tDynamicObject *) obj)->GetParticulate() != NULL))
			particulateIsEqual = false;
		if ((this->GetZernike() == NULL) && (((tDynamicObject *) obj)->GetZernike() != NULL))
			zernikeIsEqual = false;
		// join individual tests
		if (histogramIsEqual && metrichistogramIsEqual && textureIsEqual && particulateIsEqual && zernikeIsEqual
				&& soundtexturestftIsEqual && soundtexturemfccIsEqual)
			return true;
		else
			return false;
	}

	/**
	 * Returns the object indentification
	 */
	virtual long GetOID() const {
		return OID;
	}

	/**
	 * Sets the object indentification
	 */
	void SetOID(long oid) {
		OID = oid;
	}

	stSize GetSerializedSize() {
		stSize size = 0;

		// filename
		size += sizeof(char);
		size += FileName.length();

		size += sizeof(unsigned char); // size of dynamicTypes
		size += sizeof(OID); // size of OID

		// histogram
		if (this->GetHistogram() != NULL) {
			size += sizeof(stSize); // size of the GetHistogram()->GetSerializedSize()
			size += this->GetHistogram()->GetSerializedSize();
		}

		// metric histogram
		if (this->GetMetricHistogram() != NULL) {
			size += sizeof(stSize); // size of the GetMetricHistogram()->GetSerializedSize()
			size += this->GetMetricHistogram()->GetSerializedSize();
		}

		// texture
		if (this->GetTexture() != NULL) {
			size += sizeof(stSize); // size of the GetTexture()->GetSerializedSize()
			size += this->GetTexture()->GetSerializedSize();
		}

		// particulate
		if (this->GetParticulate() != NULL) {
			size += sizeof(stSize); // size of the GetParticulate()->GetSerializedSize()
			size += this->GetParticulate()->GetSerializedSize();
		}

		// zernike
		if (this->GetZernike() != NULL) {
			size += sizeof(stSize); // size of the GetTexture()->GetSerializedSize()
			size += this->GetZernike()->GetSerializedSize();
		}
		return size;
	}

	const stByte *
	Serialize() {

		// dynamicTypes is the first byte of the serialized object.
		// It determines determines which objects are stored in serialization
		// (among histogram,metric histogram and others)
		unsigned char dynamicTypes = 0;

		// is there a Histogram?
		if (this->GetHistogram() != NULL) {
			dynamicTypes += 128; // pow(2,7)
		}

		// is there a MetricHistogram?
		if (this->GetMetricHistogram() != NULL) {
			dynamicTypes += 64; // pow(2,6)
		}

		// is there a Texture?
		if (this->GetTexture() != NULL) {
			dynamicTypes += 32; // pow(2,5)
		}

		// is there a Particulate?
		if (this->GetParticulate() != NULL) {
			dynamicTypes += 16; // pow(2,4)
		}

		// is there a Zernike?
		if (this->GetZernike() != NULL) {
			dynamicTypes += 8; // pow(2,3)
		}

		// Allocate resources.
		stByte * serialized, *tmp;
		unsigned int size = GetSerializedSize();
		serialized = new stByte[size];
		tmp = serialized;

		// filename
		unsigned int fns = FileName.length();
		memcpy(tmp, &fns, sizeof(fns));
		tmp += sizeof(fns);
		memcpy(tmp, FileName.c_str(), fns);
		tmp += sizeof(char) * (FileName.length());

		memcpy(tmp, &OID, sizeof(long));
		tmp += sizeof(long);

		memcpy(tmp, &dynamicTypes, sizeof(unsigned char));
		tmp += sizeof(unsigned char);

		// Serializing the dynamic types.
		allocateDynamicType(tmp, this->GetHistogram());
		allocateDynamicType(tmp, this->GetMetricHistogram());
		allocateDynamicType(tmp, this->GetParticulate());
		allocateDynamicType(tmp, this->GetTexture());
		allocateDynamicType(tmp, this->GetZernike());

		return serialized;

	}

	void allocateDynamicType(stByte* tmp, tDynamicType* object) {
		// copy the size
		if (object != NULL) {
			int size = object->GetSerializedSize();
			memcpy(tmp, &size, sizeof(int));
			tmp += sizeof(int);
			memcpy(tmp, object->Serialize(), size);
			tmp += size;
		}
	}

	virtual void Unserialize(const stByte *data, stSize datasize) {
		const stByte * dataObject = data;

		// dynamicTypes is the first byte of the serialized object.
		// It determines which objects are stored in serialization
		// (among histogram,metric histogram and others)

		// filename
		char *fname;
		unsigned int fnamesize;
		memcpy(&fnamesize, dataObject, sizeof(fnamesize));
		fname = new char[fnamesize - 1];
		dataObject += sizeof(fnamesize);
		memcpy(fname, dataObject, fnamesize);
		fname[fnamesize] = '\0';

		FileName = string(fname);
		dataObject += fnamesize;

		memcpy(&OID, dataObject, sizeof(long));
		dataObject += sizeof(long);

		// Unserializing the types
		unsigned char dynamicTypes = *dataObject;
		dataObject += sizeof(dynamicTypes);

		// is there a Histogram?
		if (dynamicTypes >= 128) {
			// subtracting the type
			dynamicTypes -= 128;

			// instantiate the new object
			cleanDynamicType(this->GetHistogram());
			tDynamicHistogram *h = new tDynamicHistogram();

			// getting the size
			unserializeDynamicType(dataObject, h);

			// setting the object
			this->SetHistogram(h);
		}

		// is there a Metric Histogram?
		if (dynamicTypes >= 64) {
			// subtracting the type
			dynamicTypes -= 64;

			// instantiate the new object
			cleanDynamicType(this->GetMetricHistogram());
			tDynamicMetricHistogram *h = new tDynamicMetricHistogram();

			unserializeDynamicType(dataObject, h);

			// setting the object
			this->SetMetricHistogram(h);
		}

		// is there a Texture?
		if (dynamicTypes >= 32) {
			// subtracting the type
			dynamicTypes -= 32;

			// instantiate the new object
			cleanDynamicType(this->GetTexture());
			tDynamicTexture *h = new tDynamicTexture();

			unserializeDynamicType(dataObject, h);

			// setting the object
			this->SetTexture(h);
		}

		// is there a Particulate?
		if (dynamicTypes >= 16) {
			// subtracting the type
			dynamicTypes -= 16;

			// instantiate the new object
			//cleanDynamicType(this->GetParticulate());
			if (particulate != NULL) {
				delete particulate;
				//particulate = NULL;
			}
			tDynamicParticulate *h = new tDynamicParticulate();

			unserializeDynamicType(dataObject, h);
			// setting the object
			this->SetParticulate(h);
		}

		// is there a Zernike?
		if (dynamicTypes >= 8) {
			// subtracting the type
			dynamicTypes -= 8;

			cleanDynamicType(this->GetZernike());
			tDynamicZernike *h = new tDynamicZernike();

			unserializeDynamicType(dataObject, h);

			// setting the object
			this->SetZernike(h);
		}
	}

	void unserializeDynamicType(const stByte *data, tDynamicType* object) {
		int size;
		memcpy(&size, data, sizeof(int));
		data += sizeof(int);
		object->Unserialize(data, size);
		data += size;
	}

	void cleanDynamicType(tDynamicType *object) {
		if (object != NULL) {
			delete object;
			object = NULL;
		}
	}

};
//end tDynamicObject

#endif
