//---------------------------------------------------------------------------

#include "MetricTreeManager.h"

//---------------------------------------------------------------------------

// Constructor
MetricTreeManager::MetricTreeManager()
{
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
MetricTreeManager::~MetricTreeManager()
{
    if (SlimTree != NULL)
        {
            delete SlimTree;
            SlimTree = NULL;
        }
    if (DummyTree != NULL)
        {
            delete DummyTree;
            DummyTree = NULL;
        }
    if (PageManagerSlim != NULL)
        {
            delete PageManagerSlim;
            PageManagerSlim = NULL;
        }
    if (PageManagerDummy != NULL)
        {
            delete PageManagerDummy;
            PageManagerDummy = NULL;
        }
}
//---------------------------------------------------------------------------

// Loads an index file
DynamicSlimTree *MetricTreeManager::OpenSlimTree(std::string indexfile, int LpP)
{

    // adding a path to the file name
    char directory[60];
    indexfile = getcwd(directory,sizeof(directory)) +  user + "_slim_" + indexfile;

    // if index is already opened, do nothing
    if ((IndexFileSlim != indexfile) || (MetricSlim != LpP))
        {

            if (SlimTree != NULL)
                {
                    delete SlimTree;
                    SlimTree = NULL;
                }

            if (PageManagerSlim != NULL)
                {
                    delete PageManagerSlim;
                    PageManagerSlim = NULL;
                }

            FILE * fp = fopen(indexfile.c_str(),"r");
            if (fp)
                {
                    // if index file exists, open the index
                    PageManagerSlim = new stPlainDiskPageManager(indexfile.c_str());
                }
            else
                {
                    // otherwise, create a new index
                    PageManagerSlim = new stPlainDiskPageManager(indexfile.c_str(), 65536);
                }
            SlimTree = new DynamicSlimTree(PageManagerSlim);
            IndexFileSlim = indexfile;

            // Setting the metric
            tDynamicDistanceEvaluator *evaluator = SlimTree->GetMetricEvaluator();
            evaluator->SetMetric(LpP);
            MetricSlim = LpP;
        }

    return SlimTree;
}

//---------------------------------------------------------------------------
// Loads an index file
DynamicDummyTree *MetricTreeManager::OpenDummyTree(std::string indexfile, int LpP)
{

    // adding a path to the file name
    char directory[60];
    indexfile = getcwd(directory, sizeof(directory)) + user + "_dummy_" + indexfile;

    // if index is already opened, do nothing
    if ((IndexFileDummy != indexfile) || (MetricDummy != LpP))
        {

            if (DummyTree != NULL)
                {
                    delete DummyTree;
                    DummyTree = NULL;
                }
            if (PageManagerDummy != NULL)
                {
                    delete PageManagerDummy;
                    PageManagerDummy = NULL;
                }

            FILE * fp = fopen(indexfile.c_str(),"r");
            if (fp)
                {
                    // if index file exists, open the index
                    PageManagerDummy = new stPlainDiskPageManager(indexfile.c_str());
                }
            else
                {
                    // otherwise, create a new index
                    PageManagerDummy = new stPlainDiskPageManager(indexfile.c_str(), 65536);
                }
            DummyTree = new DynamicDummyTree(PageManagerDummy);
            IndexFileDummy = indexfile;

            // Setting the metric
            tDynamicDistanceEvaluator *evaluator = DummyTree->GetMetricEvaluator();
            evaluator->SetMetric(LpP);
            MetricDummy = LpP;
        }

    return DummyTree;
}

//---------------------------------------------------------------------------



