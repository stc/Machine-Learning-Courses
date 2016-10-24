#include "ofApp.h"

void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
	leap.open();
    
    leapView.allocate(300, 200, GL_RGBA);
    leapView.begin();
    ofClear(255,255,255, 0);
    leapView.end();
	cam.setOrientation(ofPoint(-20, 0, 0));
	//glEnable(GL_DEPTH_TEST);
    //glEnable(GL_NORMALIZE);
    
    camWidth = 640;  // try to grab at this size.
    camHeight = 480;
    
    //we can now get back a list of devices.
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(camWidth, camHeight);
    
    videoPixels.allocate(camWidth, camHeight, OF_PIXELS_RGB);
    videoTexture.allocate(videoPixels);
}

void ofApp::update(){
    vidGrabber.update();
    
    ofPixels & pixels = vidGrabber.getPixels();
    for(int i = 0; i < pixels.size(); i++){
        videoPixels[i] = pixels[i];
    }
    
    for (int i = 0; i < videoPixels.size(); i+=3){
        int r = videoPixels[i+0], g = videoPixels[i+1], b = videoPixels[i+2];
        unsigned char avg = (r + g + b) / 3;
        videoPixels[i+0] = avg;
        videoPixels[i+1] = avg;
        videoPixels[i+2] = avg;
    }  

    videoTexture.loadData(videoPixels);

	fingersFound.clear();
	
	simpleHands = leap.getSimpleHands();
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-230, 230, -leapView.getWidth()/2, leapView.getWidth()/2);
		leap.setMappingY(90, 490, -leapView.getHeight()/2, leapView.getHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
        
        
        for(int i = 0; i < simpleHands.size(); i++){
            for (int f=0; f<5; f++) {
                int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                fingersFound.push_back(id);
            }
        }
    }
    leap.markFrameAsOld();
}

void ofApp::draw(){
    ofBackgroundGradient(ofColor(0), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
    
    ofPushMatrix();
    ofSetRectMode( OF_RECTMODE_CENTER );
    ofTranslate( ofGetWidth()/2, ofGetHeight()/2, 0 );
    ofScale( -1, 1, 1 );
    videoTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
    ofSetRectMode( OF_RECTMODE_CORNER );
    ofPopMatrix();
    
    ofSetColor(200);
	ofDrawBitmapString("Connected? " + ofToString(leap.isConnected()), 20, 20);
    
    drawLeapView(20,ofGetHeight()-220);
}

void ofApp::drawLeapView(int left, int top) {
    leapView.begin();
    cam.begin();
    ofBackground(0);
    ofPushMatrix();
    ofRotate(90, 0, 0, 1);
    ofSetColor(120);
    ofDrawGridPlane(800, 20, false);
    ofPopMatrix();
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    for(int i = 0; i < simpleHands.size(); i++){
        bool isLeft        = simpleHands[i].isLeft;
        ofPoint handPos    = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;
        
        ofSetColor(255,100);
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 10);
        ofSetColor(255, 100);
        ofDrawArrow(handPos, handPos + 100*handNormal);
        
        for (int f=0; f<5; f++) {
            ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
            
            ofSetColor(255,180);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, 4);
            ofDrawSphere(pip.x, pip.y, pip.z, 4);
            ofDrawSphere(dip.x, dip.y, dip.z, 4);
            ofDrawSphere(tip.x, tip.y, tip.z, 4);
            
            ofSetColor(255,100);
            ofSetLineWidth(10);
            ofDrawLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
            ofDrawLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofDrawLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
        }
    }
    cam.end();
    leapView.end();
    leapView.draw(left, top);
}

void ofApp::keyPressed(int key){}
void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y ){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}

void ofApp::exit(){
    leap.close();
}
