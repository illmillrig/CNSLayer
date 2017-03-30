#pragma once

#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include <vector>


class CNSLayer : public MPxNode {
public:
	CNSLayer();
	~CNSLayer();
	virtual MPxNode::SchedulingType schedulingType() const override {return MPxNode::kParallel;}
	static void* creator();
	static MStatus initialize();
	virtual MStatus compute(const MPlug &plug, MDataBlock &data) override;
    void setData(MDataBlock &data, const std::vector<double> &weightValues);
    void processWeights(MArrayDataHandle &inWeightArrayhandle, std::vector<double> &weightValues, const int numInputs);

public:
	static MTypeId id;
	static MObject inWeight;
	static MObject outWeight;
};