#include "KinectUtil.h"



KinectUtil::KinectUtil()
{
}


KinectUtil::~KinectUtil()
{
}


 int KinectUtil::countVisibleUsers(ofxKinectCommonBridge& kinect);
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
	int noTrack = 0;
	if (skeleton.at(NUI_SKELETON_POSITION_HEAD).getTrackingState() == SkeletonBone::NotTracked)
	{
		noTrack++;
	}
	if (skeleton.at(NUI_SKELETON_POSITION_SHOULDER_LEFT).getTrackingState() == SkeletonBone::NotTracked)
		noTrack++
	}
	if (skeleton.at(NUI_SKELETON_POSITION_SHOULDER_RIGHT).getTrackingState() == SkeletonBone::NotTracked)
		noTrack++
	}
	if (skeleton.at(NUI_SKELETON_POSITION_HIP_CENTER).getTrackingState() == SkeletonBone::NotTracked)
		noTrack++
	}
	if (i >= 2) return false;
	return true;
}