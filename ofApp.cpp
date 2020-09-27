#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openframeworks");

	ofBackground(239);
	ofEnableDepthTest();

	this->cap.open("D:\\MP4\\production ID_3761468.mp4");
	this->cap_size = cv::Size(640, 360);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->frame_list.push_back(tmp);
	}

	this->mesh_frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

//--------------------------------------------------------------
void ofApp::update() {

	this->mesh.clear();
	this->mesh_frame.clear();

	int n = (int)(ofGetFrameNum() * 0.54) % this->number_of_frames;
	this->frame_list[n].copyTo(this->frame);
	this->image.update();

	int span = 10;
	for (int x = this->cap_size.width * -0.5; x < this->cap_size.width * 0.5; x += span) {

		auto angle = ofMap(ofNoise(x * 0.003 + ofGetFrameNum() * 0.02), 0, 1, -PI * 2, PI * 2);
		auto rotation = glm::rotate(glm::mat4(), angle, glm::vec3(1, 0, 0));

		auto next_angle = ofMap(ofNoise((x + span) * 0.003 + ofGetFrameNum() * 0.02), 0, 1, -PI * 2, PI * 2);
		auto next_rotation = glm::rotate(glm::mat4(), next_angle, glm::vec3(1, 0, 0));

		for (int y = this->cap_size.height * -0.5; y < this->cap_size.height * 0.5; y += span) {

			int index = this->mesh.getNumVertices();

			vector<glm::vec3> vertices;
			vertices.push_back(glm::vec4(x, y, 0, 0) * rotation);
			vertices.push_back(glm::vec4(x + span, y, 0, 0) * next_rotation);
			vertices.push_back(glm::vec4(x + span, y + span, 0, 0) * next_rotation);
			vertices.push_back(glm::vec4(x, y + span, 0, 0) * rotation);

			this->mesh.addVertices(vertices);

			this->mesh.addTexCoord(glm::vec3(x + this->cap_size.width * 0.5, y + this->cap_size.height * 0.5, 0));
			this->mesh.addTexCoord(glm::vec3(x + span + this->cap_size.width * 0.5, y + this->cap_size.height * 0.5, 0));
			this->mesh.addTexCoord(glm::vec3(x + span + this->cap_size.width * 0.5, y + span + this->cap_size.height * 0.5, 0));
			this->mesh.addTexCoord(glm::vec3(x + this->cap_size.width * 0.5, y + span + this->cap_size.height * 0.5, 0));

			mesh.addIndex(index + 0); mesh.addIndex(index + 1); mesh.addIndex(index + 2);
			mesh.addIndex(index + 0); mesh.addIndex(index + 2); mesh.addIndex(index + 3);

			if (x == this->cap_size.width * -0.5) {

				int mesh_frame_index = this->mesh_frame.getNumVertices();

				this->mesh_frame.addVertex(vertices[0]);
				this->mesh_frame.addVertex(vertices[3]);

				this->mesh_frame.addIndex(mesh_frame_index + 0);
				this->mesh_frame.addIndex(mesh_frame_index + 1);
			}

			if (x == this->cap_size.width * 0.5 - span) {

				int mesh_frame_index = this->mesh_frame.getNumVertices();

				this->mesh_frame.addVertex(vertices[1]);
				this->mesh_frame.addVertex(vertices[2]);

				this->mesh_frame.addIndex(mesh_frame_index + 0);
				this->mesh_frame.addIndex(mesh_frame_index + 1);
			}

			if (y == this->cap_size.height * -0.5) {

				int mesh_frame_index = this->mesh_frame.getNumVertices();

				this->mesh_frame.addVertex(vertices[0]);
				this->mesh_frame.addVertex(vertices[1]);

				this->mesh_frame.addIndex(mesh_frame_index + 0);
				this->mesh_frame.addIndex(mesh_frame_index + 1);
			}

			if (y == this->cap_size.height * 0.5 - span) {

				int mesh_frame_index = this->mesh_frame.getNumVertices();

				this->mesh_frame.addVertex(vertices[2]);
				this->mesh_frame.addVertex(vertices[3]);

				this->mesh_frame.addIndex(mesh_frame_index + 0);
				this->mesh_frame.addIndex(mesh_frame_index + 1);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(180);

	ofSetColor(255);
	this->image.bind();
	this->mesh.draw();
	this->image.unbind();

	ofSetColor(39);
	this->mesh_frame.drawWireframe();

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}