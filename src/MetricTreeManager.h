/**
 * @file
 *
 * This file implements a simple metric tree manager.
 *
 * @version 1.0
 *
 * @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
 * @author Humberto Razente (hlr@icmc.usp.br)
 * @todo Documentation review and tests.
 */

//---------------------------------------------------------------------------
#ifndef __METRIC_TREE_MANAGER_H
#define __METRIC_TREE_MANAGER_H
//---------------------------------------------------------------------------
#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stSlimNode.h>
#include <arboretum/stSlimTree.h>
#include <arboretum/stDummyTree.h>

#include <boost/filesystem.hpp>

#include "dynamictypes/stDynamicObject.h"
#include "dynamictypes/stDynamicEvaluator.h"

using namespace boost;

/**
 * Typedef declarations
 */
typedef stResult < tDynamicObject > DynamicResult;
typedef stJoinedResult < tDynamicObject > DynamicJoinedResult;
typedef stSlimTree < tDynamicObject, tDynamicDistanceEvaluator > DynamicSlimTree;
typedef stDummyTree < tDynamicObject, tDynamicDistanceEvaluator > DynamicDummyTree;

/**
 * This class implements a simple metric tree manager
 *
 * @author Maria Camila Nardini Barioni (mcamila@icmc.usp.br)
 * @author Humberto Razente (hlr@icmc.usp.br)
 */
class MetricTreeManager
{
public:
  /**
   * Constructor
   */
  MetricTreeManager ();

  /**
   * Destructor
   */
  ~MetricTreeManager ();

  /**
   * Loads an index file
   * @param indexfile the name of the slim tree index file
   * @param LpP the metric used
   */
  DynamicSlimTree * OpenSlimTree (std::string indexfile, int LpP);

  /**
   * Loads an index file
   * @param indexfile the name of the slim tree index file
   * @param LpP the metric used
   */
  DynamicDummyTree * OpenDummyTree (std::string indexfile, int LpP);

  /**
   * get slim disk page manager pointer
   */
  stPlainDiskPageManager *
  GetPageManagerSlim ()
  {
    return PageManagerSlim;
  }

  /**
   * get dummy disk page manager pointer
   */
  stPlainDiskPageManager *
  GetPageManagerDummy ()
  {
    return PageManagerDummy;
  }

private:
  /**
   * User name
   */
  std::string user;

  /**
   * arboretum disk page manager pointer
   */
  stPlainDiskPageManager *PageManagerSlim;

  /**
   * arboretum disk page manager pointer
   */
  stPlainDiskPageManager *PageManagerDummy;

  /**
   * arboretum slim tree pointer
   */
  DynamicSlimTree * SlimTree;

  /**
   * arboretum dummy tree pointer
   */
  DynamicDummyTree * DummyTree;

  /**
   * the name of an index
   */
  std::string IndexFileSlim;

  /**
   * the name of an index
   */
  std::string IndexFileDummy;

  /**
   * the metric
   */
  int MetricSlim;
  int MetricDummy;
};

#endif
