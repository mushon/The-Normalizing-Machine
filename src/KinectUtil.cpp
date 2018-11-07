#include "KinectUtil.h"



KinectUtil::KinectUtil()
{
}


KinectUtil::~KinectUtil()
{
}

int KinectUtil::countVisibleUsers(ofxKinectCommonBridge & kinect)
{
	int nVisibleUsers = 0;
	auto skeletons = kinect.getSkeletons();
	for (auto & skeleton : skeletons) {
		if (!skeleton.empty()) {
			if (checkMainJointsConfidence(skeleton))
			{
				nVisibleUsers++;
			}
		}
	}

	return nVisibleUsers;
}


bool KinectUtil::checkMainJointsConfidence(Skeleton& skeleton)
{
	//if (skeleton.at(NUI_SKELETON_POSITION_HEAD).getTrackingState() == SkeletonBone::NotTracked) {
	//	noTrack ++;
	//}
	auto & jrs = skeleton.at(NUI_SKELETON_POSITION_SHOULDER_RIGHT);
	auto & jls = skeleton.at(NUI_SKELETON_POSITION_SHOULDER_LEFT);
	auto & jt = skeleton.at(NUI_SKELETON_POSITION_SPINE);

	bool oneShoulderOK = (jrs.getTrackingState() == SkeletonBone::Tracked || jls.getTrackingState() == SkeletonBone::Tracked);
	return (jt.getTrackingState() == SkeletonBone::Tracked && oneShoulderOK);
}