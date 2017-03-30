
#include "CNSLayer.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <vector>


MTypeId CNSLayer::id(0x00121BD7);
MObject CNSLayer::inWeight;
MObject CNSLayer::outWeight;

CNSLayer::CNSLayer() { }
CNSLayer::~CNSLayer() { }

void* CNSLayer::creator() {
	return new CNSLayer();
}

MStatus CNSLayer::initialize() {
    MStatus stat;
    MFnNumericAttribute fnNum;

    inWeight = fnNum.create("inWeight", "inw", MFnNumericData::kDouble, 0.0, &stat);
    CHECK_MSTATUS_AND_RETURN_IT(stat);
    fnNum.setKeyable(true);
    fnNum.setArray(true);
    fnNum.setUsesArrayDataBuilder(true);
    stat = CNSLayer::addAttribute(inWeight);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    outWeight = fnNum.create("outWeight", "outw", MFnNumericData::kDouble, 0.0, &stat);
    CHECK_MSTATUS_AND_RETURN_IT(stat);
    fnNum.setStorable(false);
    fnNum.setArray(true);
    fnNum.setUsesArrayDataBuilder(true);
    stat = CNSLayer::addAttribute(outWeight);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    stat = attributeAffects(inWeight, outWeight);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    return MS::kSuccess;
}

MStatus CNSLayer::compute(const MPlug &plug, MDataBlock &data) {
    if (plug != outWeight)
        return MS::kUnknownParameter;

    MStatus stat;

    // get the inWeight array handle
    MArrayDataHandle inWeightArrayHandle = data.inputArrayValue(inWeight, &stat);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    // number of weight inputs
    unsigned int numInputs = inWeightArrayHandle.elementCount(&stat);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    // create a vector for the number of inputs
    std::vector<double> weightValues (numInputs);

    // need to loop over the inWeights in reversed order, because the last array item is the top most layer
    // remember that the handle is now at the end of the array

    stat = inWeightArrayHandle.jumpToArrayElement(numInputs - 1);
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    double topLayerWeight = inWeightArrayHandle.inputValue(&stat).asDouble();
    CHECK_MSTATUS_AND_RETURN_IT(stat);

    // min/max the topLayerWeight between 0-1
    weightValues[numInputs - 1] = std::min(1.0, std::max(0.0, topLayerWeight));

    // if the top layer is 1.0, we don't really need to do anything
    if (topLayerWeight < 1.0)
        // process the output weights from the inputs weights, in a layered fashion, starting from the bottom
        this->processWeights(inWeightArrayHandle, weightValues, numInputs);

    // set the weights
    this->setData(data, weightValues);

    return MS::kSuccess;
}

void CNSLayer::setData(MDataBlock &data, const std::vector<double> &weightValues) {
    MArrayDataHandle outWeightArrayHandle = data.outputArrayValue(outWeight);

    size_t numValues = weightValues.size();
    for (unsigned int i = 0; i < numValues; ++i){
        outWeightArrayHandle.jumpToArrayElement(i);
        outWeightArrayHandle.outputValue().setDouble(weightValues[i]);
    }

    data.setClean(outWeight);
}

void CNSLayer::processWeights(MArrayDataHandle &inWeightArrayhandle, std::vector<double> &weightValues, const int numInputs) {
    MStatus stat;

    // the amount of available weight for
    double availableWeight = 1.0 - weightValues[numInputs-1];

    // loop over in reverse, except for the last item, which is topLayerWeight
    for (int i = numInputs-2 ; i >= 0 ; --i){
        stat = inWeightArrayhandle.jumpToArrayElement((unsigned)i);
        CHECK_MSTATUS(stat);
        weightValues[i] = std::min(1.0, std::max(0.0, inWeightArrayhandle.inputValue().asDouble()));
        weightValues[i] *= availableWeight;
        availableWeight -= weightValues[i];
    }

}

























