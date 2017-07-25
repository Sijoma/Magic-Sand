//
//  Model.cpp
//  Magic-Sand
//
//  Created by Jan on 06/06/17.
//
//

#include "Model.h"

Model::Model(std::shared_ptr<KinectProjector> const& k){
    kinectProjector = k;
    timestep = 0;
    
    
    // Retrieve variables
    kinectROI = kinectProjector->getKinectROI();
    resetBurnedArea();

}

bool Model::isRunning() {
	return fires.size() > 0 || embers.size() > 0;
}

void Model::addNewFire(){
    ofVec2f location;
    setRandomVehicleLocation(kinectROI, false, location);
    auto f = Fire(kinectProjector, location, kinectROI);
    f.setup();
    fires.push_back(f);
}

void Model::addNewFire(ofVec2f fireSpawnPos) {
    addNewFire(fireSpawnPos, windDirection);
}

void Model::addNewFire(ofVec2f fireSpawnPos, float angle){
    if (kinectProjector->elevationAtKinectCoord(fireSpawnPos.x, fireSpawnPos.y) < 0){
        return;
    }
    auto f = Fire(kinectProjector, fireSpawnPos, kinectROI, angle);
    f.setup();
    fires.push_back(f);
}

void Model::addNewFireInRiskZone(){
    if (riskZones.size() == 0){
        calculateRiskZones();
    }
    ofVec2f spawnPosition;
    ofRectangle borders = kinectProjector->getKinectROI();
    borders.scaleFromCenter((borders.width-50)/borders.width, (borders.height-50)/borders.height);
    int counter = 0;
    do {
        int index = std::rand() % riskZones.size();
        ofVec2f spawnPosition = riskZones[index];
        counter++;
    } while (!borders.inside(spawnPosition)&& counter <= 100);
    
    addNewFire(spawnPosition);
}

bool Model::setRandomVehicleLocation(ofRectangle area, bool liveInWater, ofVec2f & location){
    bool okwater = false;
    int count = 0;
    int maxCount = 100;
    while (!okwater && count < maxCount) {
        count++;
        float x = ofRandom(area.getLeft(),area.getRight());
        float y = ofRandom(area.getTop(),area.getBottom());
        bool insideWater = kinectProjector->elevationAtKinectCoord(x, y) < 0;
        if ((insideWater && liveInWater) || (!insideWater && !liveInWater)){
            location = ofVec2f(x, y);
            okwater = true;
        }
    }
    return okwater;
}

// SETTERS and GETTERS Fire Parameters:
void Model::setWindSpeed(float v) {
	windSpeed = v;
}

void Model::setWindDirection(float d) {
	windDirection = d;
}

void Model::update(){
    // kinectROI updated
    if (kinectProjector->getKinectROI() != kinectROI){
        kinectROI = kinectProjector->getKinectROI();
        resetBurnedArea();
    }
    
    //spread fires
    int size = fires.size();
    int i = 0;
    while(i < size){
        embers.push_back(fires[i]);
        ofPoint location = fires[i].getLocation();
        if (burnedArea[floor(location.x)][floor(location.y)] || !fires[i].isAlive()){
            fires.erase(fires.begin() + i);
            size--;
        } else {
            burnedArea[floor(location.x)][floor(location.y)] = true;
			burnedAreaCounter += 1;
            int rand = std::rand() % 100;
            int spreadFactor = timestep < 10 ? 70 : 10;
            if (fires[i].isAlive() && rand < spreadFactor){
                int angle = fires[i].getAngle();
                addNewFire(location, (angle + 90)%360);
                addNewFire(location, (angle + 270)%360);
            }
            i++;
        }
    }
    
    for (auto & f : fires){
        f.applyBehaviours(windSpeed, windDirection);
        f.update();
    }
    timestep++;
}

void Model::draw(){
    drawEmbers();
    for (auto & f : fires){
        f.draw();
    }
}

void Model::clear(){
    fires.clear();
	embers.clear();
    timestep = 0;
    resetBurnedArea();
}

void Model::resetBurnedArea(){
    burnedArea.clear();
	burnedAreaCounter = 0;
	completeArea = 0;
    for(int x = 0; x <= kinectROI.getRight(); x++ ){
        vector<bool> row;
        for (int y = 0; y <= kinectROI.getBottom(); y++ ){
            row.push_back(false);
			completeArea += 1;
        }
        burnedArea.push_back(row);
    }
}

void Model::calculateRiskZones() {
    riskZones.clear();
	for (int x = kinectROI.getLeft() + 1; x < kinectROI.getRight(); x++) {
		for (int y = kinectROI.getTop() + 1; y < kinectROI.getBottom(); y++) {
			float cell_aspect;
			float cell_slope;
			//assignment of the neighborhood
			float a = kinectProjector->elevationAtKinectCoord(x - 1, y - 1);
			float b = kinectProjector->elevationAtKinectCoord(x, y - 1);
			float c = kinectProjector->elevationAtKinectCoord(x + 1, y - 1);
			float d = kinectProjector->elevationAtKinectCoord(x - 1, y);
			float e = kinectProjector->elevationAtKinectCoord(x, y);
			float f = kinectProjector->elevationAtKinectCoord(x + 1, y);
			float g = kinectProjector->elevationAtKinectCoord(x - 1, y + 1);
			float h = kinectProjector->elevationAtKinectCoord(x, y + 1);
			float i = kinectProjector->elevationAtKinectCoord(x + 1, y + 1);
			float changeRateInXDirection = ((c + 2 * f + i) - (a + 2 * d + g)) / 8;
			float changeRateInYDirection = ((g + 2 * h + i) - (a + 2 * b + c)) / 8;
			//calculation of south aspects
			float aspect = 180 / PI * atan2(changeRateInYDirection, -changeRateInXDirection);
			if (aspect < 0) {
				cell_aspect = 90.0 - aspect;
			}
			else if (aspect > 90.0) {
				cell_aspect = 360.0 - aspect + 90.0;
			}
			else {
				cell_aspect = 90.0 - aspect;
			}
			//calculation of slopes >= 10 degrees
			cell_slope = atan(sqrt(pow(changeRateInXDirection, 2) + pow(changeRateInYDirection, 2))) * (180 / PI);

			//identification of risk zones
			if (cell_aspect >= 157.5 && cell_aspect <= 202.5 && cell_slope >= 10) {
				riskZones.push_back(ofVec2f(x, y));
			}
		}
	}
}

void Model::drawEmbers(){
    int i = 0;
    int size = embers.size();
    while(i < size){
        if(embers[i].isAlive()){
            embers[i].kill();
        }
        embers[i].draw();
        if(embers[i].getIntensity() <= 0){
            embers.erase(embers.begin() + i);
            size--;
        } else {
            i++;
        }
    }
}

void Model::drawRiskZones() {
    for (auto & r : riskZones){
        ofPoint coord = kinectProjector->kinectCoordToProjCoord(r.x, r.y);
        ofFill();
        
        ofPath riskZone;
        riskZone.rectangle(coord.x - 2, coord.y - 2, 4, 4);
        riskZone.setFillColor(ofColor(255, 0, 0, 200));
        riskZone.setStrokeWidth(0);
        riskZone.draw();
        
        ofNoFill();
    }
}

string Model::getPercentageOfBurnedArea(){
	float percentage = (burnedAreaCounter / (completeArea/7)) * 100;
  percentage = percentage > 100 ? 100 : percentage;
	string percentStr = "Burned area: ";
	percentStr += std::to_string(percentage);
	percentStr += " %";
	return percentStr;
}

int Model::getNumberOfAgents(){
	return fires.size();
}

int Model::getTimestep() {
	return timestep;
}
