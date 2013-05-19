//---------------------------------------------------------------------------

#include "MetricTreeManager.h"
#include "utils/sirenUtils.h"

//---------------------------------------------------------------------------

// Constructor

MetricTreeManager::MetricTreeManager() {
	// page manager and slim tree pointers inicialization
	//user = username;
	PageManagerSlim = NULL;
	PageManagerDummy = NULL;
	SlimTree = NULL;
	DummyTree = NULL;
	IndexFileSlim = "";
	IndexFileDummy = "";
	MetricSlim = -1;
	MetricDummy = -1;
}
//---------------------------------------------------------------------------

// Destructor

MetricTreeManager::~MetricTreeManager() {
	if (SlimTree != NULL) {
		delete SlimTree;
		SlimTree = NULL;
	}
	if (DummyTree != NULL) {
		delete DummyTree;
		DummyTree = NULL;
	}
	if (PageManagerSlim != NULL) {
		delete PageManagerSlim;
		PageManagerSlim = NULL;
	}
	if (PageManagerDummy != NULL) {
		delete PageManagerDummy;
		PageManagerDummy = NULL;
	}
}
//---------------------------------------------------------------------------

// Loads an index file

DynamicSlimTree *MetricTreeManager::OpenSlimTree(std::string indexfile, int LpP) {

	// adding a path to the file name

	//@TODO Fix here.
	string path = filesystem::current_path().string();
	path += "slim_" + indexfile;

	// if index is already opened, do nothing
	if ((IndexFileSlim != indexfile) || (MetricSlim != LpP)) {

		if (SlimTree != NULL) {
			delete SlimTree;
			SlimTree = NULL;
		}

		if (PageManagerSlim != NULL) {
			delete PageManagerSlim;
			PageManagerSlim = NULL;
		}

		if (sirenUtils::getInstance().fileExists(path)) {
			// if index file exists, open the index
			PageManagerSlim = new stPlainDiskPageManager(path.c_str());
		} else {
			// otherwise, create a new index
			PageManagerSlim = new stPlainDiskPageManager(path.c_str(), 65536);
		}
		SlimTree = new DynamicSlimTree(PageManagerSlim);
		IndexFileSlim = indexfile;

		// Setting the metric
		tDynamicDistanceEvaluator *evaluator = SlimTree->GetMetricEvaluator();
		evaluator->setMetric(LpP);
		MetricSlim = LpP;
	}

	return SlimTree;
}

//---------------------------------------------------------------------------
// Loads an index file

DynamicDummyTree *MetricTreeManager::OpenDummyTree(std::string indexfile, int LpP) {

	// adding a path to the file name
	string path = filesystem::current_path().string();
	path += "dummy_" + indexfile;

	// if index is already opened, do nothing
	if ((IndexFileDummy != indexfile) || (MetricDummy != LpP)) {

		if (DummyTree != NULL) {
			delete DummyTree;
			DummyTree = NULL;
		}
		if (PageManagerDummy != NULL) {
			delete PageManagerDummy;
			PageManagerDummy = NULL;
		}
		if (sirenUtils::getInstance().fileExists(path)) {
			// if index file exists, open the index
			PageManagerDummy = new stPlainDiskPageManager(path.c_str());
		} else {
			// otherwise, create a new index
			PageManagerDummy = new stPlainDiskPageManager(path.c_str(), 65536);
		}
		DummyTree = new DynamicDummyTree(PageManagerDummy);
		IndexFileDummy = indexfile;

		// Setting the metric
		tDynamicDistanceEvaluator *evaluator = DummyTree->GetMetricEvaluator();
		evaluator->setMetric(LpP);
		MetricDummy = LpP;
	}

	return DummyTree;
}

//---------------------------------------------------------------------------

