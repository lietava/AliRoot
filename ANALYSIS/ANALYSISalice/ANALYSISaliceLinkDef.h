#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class  AliAnalysisTaskSE+;
#pragma link C++ class  AliAnalysisTaskME+;
#pragma link C++ class  AliAnalysisTaskMCParticleFilter+;
#pragma link C++ class  AliAnalysisTaskTagCreator+;
#pragma link C++ class  AliKineTrackCuts+;
#pragma link C++ class  AliESDtrackCuts+;
#pragma link C++ class  AliESDpidCuts+;
#pragma link C++ class  AliESDv0Cuts+;
#pragma link C++ class  AliESDv0KineCuts+;

#pragma link C++ class  AliEventPoolOTF+;
#pragma link C++ class  AliEventPoolLoop+;

#pragma link C++ class AliEventPoolSparse+;

#pragma link C++ class AliMultiEventInputHandler+;

#pragma link C++ class  AliAnalysisFilter+;
#pragma link C++ class  AliAnalysisCuts+;

#pragma link C++ class AliVZEROEPSelectionTask+;
#pragma link C++ class AliAnalysisTaskStat+;
#pragma link C++ class AliAnalysisTaskPIDResponse+;
#pragma link C++ class AliPIDResponseInputHandler+;
#pragma link C++ class AliHEPDataParser+;
#pragma link C++ class AliAnalysisTaskPIDqa+;
#pragma link C++ class AliAnalysisTaskBaseLine+;
#pragma link C++ class AliAnalysisTaskPIDCombined+;
#pragma link C++ class AliAODv0KineCuts;
#pragma link C++ class AliEventPool+;
#pragma link C++ class AliEventPoolManager+;
#pragma link C++ class AliUnfolding+;

#pragma link C++ class AliAnalysisTaskBadChunkID+;

#pragma link C++ class AliTrackSelectionFactory+;
#pragma link C++ class AliVTrackSelection+;
#pragma link C++ class AliESDTrackSelection+;
#pragma link C++ class AliAODTrackSelection+;

#pragma link C++ class AliAODITSsaTrackCuts+;

// If ROOT was built with XML support
#ifdef WITHXML
#pragma link C++ class AliTagAnalysis+;
#pragma link C++ class AliXMLCollection+;
#endif

#endif
