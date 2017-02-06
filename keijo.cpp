#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<cmath>


class Keijo {
private:
	int LineFilter(cv::Mat &input_mat) {
		double ver_black_blight = 0, ver_white_blight = 0;
		double hol_black_blight = 0, hol_white_blight = 0;
		double ver_ret,hol_ret;

		//縦の線と横の線の黒と白の数を数える
		//明度差があれば出力
		uchar *ptr;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++,ptr++) {
				if (x != 0 || x != input_mat.cols - 1) { ver_black_blight += *ptr; }
				else { ver_white_blight += *ptr; }
				if (x != 0 || x != input_mat.cols - 1) { hol_black_blight += *ptr; }
				else { hol_white_blight += *ptr; }
			}
		}

		ver_ret = abs(ver_white_blight - ver_black_blight);
		hol_ret = abs(hol_white_blight - hol_black_blight);

		if (ver_ret >= 200 || hol_ret >= 200) { return (int)(MAX(ver_ret, hol_ret)); }
		else { return 0; }
	}
	int PepperFilter(cv::Mat &input_mat) {
		double black_blight = 0, white_blight = 0;
		double ret = 0;

		//中心の黒と周りの白の数を数える
		//明度差があれば出力
		uchar *ptr;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				if (x == 0 || x == input_mat.cols - 1 || y == 0 || y == input_mat.rows - 1) { white_blight += *ptr; }
				else { black_blight += *ptr; }
			}
		}

		ret = abs(white_blight - black_blight);


		if (ret >= 200) { return (int)ret; }
		else { return 0; }
	}
	int CriffFilter(cv::Mat &input_mat) {
		double left_blight = 0, right_blight = 0;
		double top_blight = 0, buttom_blight = 0;
		double lr_ret = 0, tb_ret = 0;

		//左右もしくは上下で
		//明度差があれば出力
		uchar *ptr;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				if (x < input_mat.cols / 2) { left_blight += *ptr; }
				else { right_blight += *ptr; }

				if (y < input_mat.rows / 2) { top_blight += *ptr; }
				else { buttom_blight += *ptr; }
			}
		}

		lr_ret = abs(left_blight - right_blight);
		tb_ret = abs(top_blight - buttom_blight);


		if (lr_ret >= 200 || tb_ret >= 200) { return (int)(MAX(lr_ret, tb_ret)); }
		else { return 0; }
	}
	int SnakeFilter(cv::Mat &input_mat) {
		double ver_black_blight = 0, ver_white_blight = 0;
		double hol_black_blight = 0, hol_white_blight = 0;
		double ver_ret, hol_ret;

		//縦の線と横の線の黒と白の数を数える
		//どちらも高い出力値なら
		//明度差があれば出力
		uchar *ptr;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				if (x != 0 || x != input_mat.cols - 1) { ver_black_blight += *ptr; }
				else { ver_white_blight += *ptr; }
				if (x != 0 || x != input_mat.cols - 1) { hol_black_blight += *ptr; }
				else { hol_white_blight += *ptr; }
			}
		}

		ver_ret = abs(ver_white_blight - ver_black_blight);
		hol_ret = abs(hol_white_blight - hol_black_blight);

		if (ver_ret >= 200 && hol_ret >= 200) { return (int)(MAX(ver_ret, hol_ret)); }
		else { return 0; }
	}
	int KadoFilter(cv::Mat &input_mat) {
		double lt_blight = 0, rt_blight = 0;
		double lb_blight = 0, rb_blight = 0;
		double ret = 0;

		//4方向でみて1箇所のみが黒く、他が白い場合
		//出力
		uchar *ptr;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				if ((x < input_mat.cols / 2) && (y < input_mat.rows / 2)) { lt_blight += *ptr; }
				if ((x < input_mat.cols / 2) && (y > input_mat.rows / 2)) { rt_blight += *ptr; }
				if ((x > input_mat.cols / 2) && (y < input_mat.rows / 2)) { lb_blight += *ptr; }
				if ((x > input_mat.cols / 2) && (y > input_mat.rows / 2)) { rb_blight += *ptr; }
			}
		}
		bool lt = 0, lb = 0, rt = 0, rb = 0;
		if (abs(lt_blight - (lb_blight + rb_blight + rt_blight) / 3) > 210) { lt = 1; }
		if (abs(lb_blight - (lt_blight + rb_blight + rt_blight) / 3) > 210) { lb = 1; }
		if (abs(rt_blight - (lb_blight + rb_blight + lt_blight) / 3) > 210) { rt = 1; }
		if (abs(rb_blight - (lb_blight + lt_blight + rt_blight) / 3) > 210) { rb = 1; }


		if (lt || lb || rt || rb) { return (int)(this->max(input_mat) - this->min(input_mat)); }
		else { return 0; }
	}

public:

	Keijo(){}

	void LineFilterBlack(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {
		
		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = LineFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void LineFilterWhite(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = ~input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = LineFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void PepperFilterBlack(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = PepperFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void PepperFilterWhite(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = ~input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = PepperFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void CriffFilter(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = CriffFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void SnakeFilterBlack(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = SnakeFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void SnakeFilterWhite(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = ~input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = SnakeFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void KadoFilterBlack(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = KadoFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	void KadoFilterWhite(cv::Mat &input_mat, cv::Mat &output_mat, int masksize) {

		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows - masksize, input_mat.cols - masksize, CV_8U);
		cv::Mat tempmat = cv::Mat::zeros(masksize, masksize, CV_8U);

		uchar *ptr;
		for (int y = 0; y < dst_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < dst_mat.cols; x++, ptr++) {
				tempmat = ~input_mat(cv::Rect(x, y, masksize, masksize));
				*ptr = KadoFilter(tempmat);
			}
		}

		output_mat = dst_mat;
	}

	//2値化の関数
	void Gray2Binary(cv::Mat &input_mat, cv::Mat &output_mat) {
		cv::Mat dst_mat = cv::Mat::zeros(input_mat.rows, input_mat.cols, CV_8U);
		uchar *ptr;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = dst_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++,ptr++) {
				if (*ptr >= 127) { *ptr = 1; }
				else { *ptr = 0; }
			}
		}
		
		output_mat = dst_mat;
	}

	uchar max(cv::Mat &input_mat) {
		uchar *ptr;
		uchar max = 0;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				if (*ptr >= max) { max = *ptr; }
			}
		}
		return max;
	}

	uchar min(cv::Mat &input_mat) {
		uchar *ptr;
		uchar min = 0;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				if (*ptr <= min) { min = *ptr; }
			}
		}
		return min;
	}

	float mean(cv::Mat &input_mat) {
		uchar *ptr;
		float mean = 0;
		double sum = 0;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				sum += *ptr;
			}
		}
		mean = sum / (input_mat.cols * input_mat.rows);
		return mean;
	}

	double to_rms(cv::Mat &input_mat) {
		uchar *ptr;
		double rms = 0.0;
		for (int y = 0; y < input_mat.rows; y++) {
			ptr = input_mat.ptr(y);
			for (int x = 0; x < input_mat.cols; x++, ptr++) {
				rms += (*ptr)*(*ptr);
			}
		}
		rms /= (input_mat.cols*input_mat.rows);
		rms = sqrt(rms);
		return rms;
	}

	std::vector<double> rms = {};

	void push_back_rms(cv::Mat &input_mat, int masksize) {
		cv::Mat dst_img;
		this->KadoFilterBlack(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));
		this->KadoFilterWhite(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));

		this->LineFilterBlack(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));
		this->LineFilterWhite(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));

		this->SnakeFilterBlack(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));
		this->SnakeFilterWhite(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));

		this->PepperFilterBlack(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));
		this->PepperFilterWhite(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));

		this->CriffFilter(input_mat, dst_img, masksize);
		this->rms.push_back(this->to_rms(dst_img));

	}

	void set_45_rms(cv::Mat &input_mat) {
		this->rms = {};
		std::vector<int> masksize{5,7,11,15,21};
		for (auto itr = masksize.begin(); itr != masksize.end(); ++itr) {
			this->push_back_rms(input_mat, *itr);
		}
	}

};


