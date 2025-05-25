#include "stdafx.h"
#include "hisvg.h"


Node* buildDendrogram(const std::vector<Linkage>& Z, int N, double& maxH) {
	std::map<int, Node*> nodes;
	for (int i = 0; i < N; ++i) nodes[i] = new Node();
	maxH = 0;
	int id = N;
	for (auto& rec : Z) {
		Node* l = nodes[rec.idx1];
		Node* r = nodes[rec.idx2];
		Node* p = new Node();
		p->left = l;
		p->right = r;
		p->height = rec.dist;
		nodes[id] = p;
		maxH = std::max(maxH, rec.dist);
		id++;
	}
	return nodes[id - 1];
}

void assignX(Node* n, double spacing, double& cur) {
	if (!n->left) {
		n->x = cur;
		cur += spacing;
	}
	else {
		assignX(n->left, spacing, cur);
		assignX(n->right, spacing, cur);
		n->x = 0.5 * (n->left->x + n->right->x);
	}
}

void assignY(Node* n, double maxH, double H) {
	if (n->left) {
		n->y = H - (n->height / maxH) * (H - 20);
		assignY(n->left, maxH, H);
		assignY(n->right, maxH, H);
	}
	else {
		n->y = H;
	}
}

void draw(Node* n, std::ofstream& os) {
	if (!n->left) return;
	os << "<line x1='" << n->left->x << "' y1='" << n->left->y
		<< "' x2='" << n->left->x << "' y2='" << n->y
		<< "' stroke='black'/>"
		<< "<line x1='" << n->right->x << "' y1='" << n->right->y
		<< "' x2='" << n->right->x << "' y2='" << n->y
		<< "' stroke='black'/>"
		<< "<line x1='" << n->left->x << "' y1='" << n->y
		<< "' x2='" << n->right->x << "' y2='" << n->y
		<< "' stroke='black'/>\n";
	draw(n->left, os);
	draw(n->right, os);
}

void writeSVG(
	Node* root, int N, double W, double H,
	const std::string& file,
	const std::vector<std::string>& labels
) {
	if ((int)labels.size() != N) {
		std::cerr << "Error: labels size must equal N." << std::endl;
		return;
	}
	double spacing = W / (N - 1.0);
	double cur = 0.0;
	assignX(root, spacing, cur);
	assignY(root, root->height, H);

	std::ofstream svg(file);
	svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << W
		<< "' height='" << H + 30 << "'>\n"; // 底部预留 30px
	draw(root, svg);
	// 绘制叶节点标签
	for (int i = 0; i < N; ++i) {
		double x = i * spacing;
		svg << "<text x='" << x << "' y='" << H + 20
			<< "' font-size='12' text-anchor='middle'>"
			<< labels[i] << "</text>\n";
	}
	svg << "</svg>\n";
	svg.close();
}

void cleanup(Node* n) {
	if (!n) return;
	cleanup(n->left);
	cleanup(n->right);
	delete n;
}
