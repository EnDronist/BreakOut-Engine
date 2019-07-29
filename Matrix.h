#pragma once
#include <vector>
#include <iostream>
using namespace std;
class Matrix {
public:
	Matrix(int rows, int columns):
		rows(rows),
		columns(columns) {
		value.resize(rows);
		for (int i = 0; i < rows; i++) value[i].resize(columns);
	}
	Matrix(vector<vector<double>> value):
		rows((int)value.size()),
		columns((int)value[0].size()),
		value(value) {
	}
	~Matrix() {}
	void Show() const {
		for (int irows = 0; irows < rows; irows++) {
			for (int icolumns = 0; icolumns < columns; icolumns++) {
				cout << this->value[irows][icolumns] << " ";
			}
			cout << endl;
		}
	}
	vector<vector<double>> GetValue() {
		return value;
	}
	Matrix& operator=(Matrix &rhs) {
		this->value = rhs.value;
		this->rows = rhs.rows;
		this->columns = rhs.columns;
	}
	Matrix operator+(Matrix &rhs) const {
		if (this->rows == rhs.rows && this->columns == rhs.columns) {
			for (int irows = 0; irows < rhs.rows; irows++) {
				for (int icolumns = 0; icolumns < rhs.columns; icolumns++) {
					rhs.value[irows][icolumns] += this->value[irows][icolumns];
				}
			}
		}
		return rhs;
	}
	Matrix& operator+=(Matrix &rhs) {
		return *this = *this + rhs;
	}
	Matrix operator-(Matrix &rhs) const {
		if (this->rows == rhs.rows && this->columns == rhs.columns) {
			for (int irows = 0; irows < rhs.rows; irows++) {
				for (int icolumns = 0; icolumns < rhs.columns; icolumns++) {
					rhs.value[irows][icolumns] -= this->value[irows][icolumns];
				}
			}
		}
		return rhs;
	}
	Matrix& operator-=(Matrix &rhs) {
		return *this = *this - rhs;
	}
	Matrix operator*(Matrix &rhs) const {
		Matrix matrix(this->rows, rhs.columns);
		if (this->columns == rhs.rows) {
			for (int irows = 0; irows < this->rows; irows++) {
				for (int icolumns = 0; icolumns < rhs.columns; icolumns++) {
					double result = 0;
					for (int i = 0; i < this->columns; i++) {
						result += this->value[irows][i] * rhs.value[i][icolumns];
					}
					matrix.value[irows][icolumns] = result;
				}
			}
		}
		return matrix;
	}
	Matrix& operator*=(Matrix &rhs) {
		return *this = *this * rhs;
	}
	Matrix operator*(double rhs) const {
		Matrix matrix(this->rows, this->columns);
		for (int irows = 0; irows < this->rows; irows++) {
			for (int icolumns = 0; icolumns < this->columns; icolumns++) {
				matrix.value[irows][icolumns] = this->value[irows][icolumns];
				matrix.value[irows][icolumns] *= rhs;
			}
		}
		return matrix;
	}
	Matrix& operator*=(double rhs) {
		return *this = *this * rhs;
	}
private:
	vector<vector<double>> value;
	int rows;
	int columns;
};