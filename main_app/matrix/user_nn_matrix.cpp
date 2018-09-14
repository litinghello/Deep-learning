
#include "user_nn_matrix.h"
#include "user_nn_matrix_cuda.h"

//����һ������
//����
//dest��ָ����� �������ȳ�ʼ��
//width������Ŀ���
//height������ĸ߶�
user_nn_matrix *user_nn_matrix_create(int width, int height){
	user_nn_matrix *dest;

	dest = (user_nn_matrix *)malloc(sizeof(user_nn_matrix));//���䱣�����ռ�Ĵ�С
	dest->width = width;
	dest->height = height;
	dest->data = (float *)malloc(dest->width * dest->height * sizeof(float));//����������ݿռ�
	dest->next = NULL;

	memset(dest->data, 0, dest->width * dest->height * sizeof(float));//�������

	return dest;
}
//����һ������
//����
//dest��ָ����� �������ȳ�ʼ��
//width������Ŀ���
//height������ĸ߶�
//�����µľ���
user_nn_matrix *user_nn_matrix_cpy_create(user_nn_matrix *dest_matrix){
	user_nn_matrix *result=NULL;

	result = (user_nn_matrix *)malloc(sizeof(user_nn_matrix));//���䱣�����ռ�Ĵ�С
	result->width = dest_matrix->width;
	result->height = dest_matrix->height;
	result->data = (float *)malloc(result->width * result->height * sizeof(float));//����������ݿռ�
	result->next = NULL;

	memcpy(result->data,dest_matrix->data,result->width * result->height * sizeof(float));

	return result;
}
//����ת�� ���������width height��������
void user_nn_matrix_transpose(user_nn_matrix *src_matrix){
#ifdef USE_CUDA
	user_nn_matrix_transpose_cuda(src_matrix);
#else
	user_nn_matrix *temp_matrix = NULL;
	float *temp_data = NULL;
	float *src_data = src_matrix->data;
	int width = 0;
	int height = 0;

	if ((src_matrix->width == 1) || (src_matrix->height == 1)){
		//�����һ���������� ��ôֱ�ӽ����������� ���ý�������
		src_matrix->width = src_matrix->width ^ src_matrix->height;
		src_matrix->height = src_matrix->width ^ src_matrix->height;
		src_matrix->width = src_matrix->width ^ src_matrix->height;

		return;//ֱ���˳�
	}
	temp_matrix = user_nn_matrix_create(src_matrix->width, src_matrix->height);//��������
	user_nn_matrix_cpy_matrix(temp_matrix, src_matrix);//��������
	temp_data = temp_matrix->data;//��ȡ������������ָ��

	for (width = 0; width < temp_matrix->width; width++){
		for (height = 0; height < temp_matrix->height; height++){
			*src_data++ = *user_nn_matrix_ext_value(temp_matrix, width, height);
		}
	}
	src_matrix->height = width;
	src_matrix->width = height;

	user_nn_matrix_delete(temp_matrix);
#endif
}
//���ؾ�����ָ��λ�õ�ֵ
//����
//dest������
//post_index������λ��
float *user_nn_matrix_ext_value_index(user_nn_matrix *dest, int post_index){
	float *value = dest->data;
	if (post_index >= (dest->width * dest->height)){
		return NULL;
	}
	return (value + post_index);
}
//���ؾ�����ָ��λ�õ�ֵ
//����
//dest������
//postx��x����
//posty��y����
float *user_nn_matrix_ext_value(user_nn_matrix *dest, int postx, int posty){
	return user_nn_matrix_ext_value_index(dest,postx + posty * dest->width);
}
//���ؾ��������ֵ��ϵ��
//����
//dest������
int user_nn_matrix_return_max_index(user_nn_matrix *dest){
	int count = 0,max_index = 0;
	float *value = NULL;
	float max_value = *dest->data;

	count = 0; value = dest->data;
	for(;count<(dest->width * dest->height);count++){
		if (*value > max_value){
			max_value = *value;
			max_index = count;
		}
		value++;
	}
	return max_index;
}
//���ؾ��������ֵ��ָ��
//����
//dest������
float *user_nn_matrix_return_max_addr(user_nn_matrix *dest){
	int count = dest->width * dest->height;
	float *value = dest->data;
	float *result = dest->data;
	float max_value = *dest->data;
	
	while (count--){
		if (*value > max_value){
			max_value = *value;
			result = value;
		}
		value++;
	}
	return  result;
}
//���ؾ�������Сֵ��ϵ��
//����
//dest������
int user_nn_matrix_return_min_index(user_nn_matrix *dest){
	int count = 0,min_index = 0;
	float *value = dest->data;
	float min_value = *dest->data;

	for(;count<(dest->width * dest->height);count++){
		if (*value < min_value){
			min_value = *value;
			min_index = count;
		}
		value++;
	}
	return min_index;
}
//���ؾ�������Сֵ��ָ��
//����
//dest������
float *user_nn_matrix_return_min_addr(user_nn_matrix *dest){
	int count = dest->width * dest->height;
	float *value = dest->data;
	float *result = dest->data;
	float min_value = *dest->data;
	
	while (count--){
		if (*value < min_value){
			min_value = *value;
			result = value;
		}
		value++;
	}
	return  result;
}
//ɾ������
void user_nn_matrix_delete(user_nn_matrix *dest){
	if (dest != NULL){
		if (dest->data != NULL){
			free(dest->data);//�ͷ�����
		}
		free(dest);//�ͷŽṹ��
	}
	//dest = NULL;
}
//���������ľ���
//����
//total_w����������Ŀ���
//total_h����������ĸ߶�
//matrix_w matrix_h����������ĵ�������ߴ�
user_nn_list_matrix *user_nn_matrices_create(int total_w, int total_h, int matrix_w, int matrix_h){
	user_nn_list_matrix *list_matrix = NULL;//��������
	user_nn_matrix *matrix = NULL;//���������С
	int n = total_w * total_h;//������Ҫ�������ٸ�����
	if (n == 0){ return NULL; }//�����������Ϊ��
	list_matrix = (user_nn_list_matrix *)malloc(sizeof(user_nn_list_matrix));//����ռ�
	list_matrix->width = total_w;//�����ܾ������
	list_matrix->height = total_h;//�����ܾ���߶�
	list_matrix->matrix = user_nn_matrix_create(matrix_w, matrix_h);//�����׸�����
	matrix = list_matrix->matrix;//ת���������

	while (--n) {
		matrix->next = user_nn_matrix_create(matrix_w, matrix_h);//����һ������
		matrix = matrix->next;//����ָ�����
		//printf("create matrix=%d\n", width * height - n + 1);
	}
	return list_matrix;
}
//���������ľ���ͷ
//����
//total_w����������Ŀ���
//total_h����������ĸ߶�
user_nn_list_matrix *user_nn_matrices_create_head(int total_w, int total_h) {
	user_nn_list_matrix *list_matrix = NULL;//��������
	user_nn_matrix *matrix = NULL;//���������С
	list_matrix = (user_nn_list_matrix *)malloc(sizeof(user_nn_list_matrix));//����ռ�
	list_matrix->width = total_w;//�����ܾ������
	list_matrix->height = total_h;//�����ܾ���߶�
	list_matrix->matrix = NULL;//�����׸�����
	return list_matrix;
}
//ɾ����������
//src_matrices�������������
//����ֵ����
void user_nn_matrices_delete(user_nn_list_matrix *src_matrices){
	int count = src_matrices->width * src_matrices->height;//��ȡ�ܾ����С
	user_nn_matrix *matrix = src_matrices->matrix;//ָ�򵥸�����
	user_nn_matrix *matrix_next = NULL;

	while (matrix != NULL){
		matrix_next = matrix->next;
		user_nn_matrix_delete(matrix);//ɾ����ǰ����
		matrix = matrix_next;//���¾���
	}
}
//������������ĩβ����һ������ ���� ������������ǵ��л��ߵ��о��� 
//����
//list_matrix����������
//end_matirx����Ҫ���ӵľ���
//���� �ɹ�ʧ��
bool user_nn_matrices_add_matrix(user_nn_list_matrix *list_matrix,user_nn_matrix *end_matirx) {
	user_nn_matrix *matrix = list_matrix->matrix;//��ȡ��һ���������
	if ((list_matrix->height != 1) && (list_matrix->width != 1)) {
		return false;//���Ӿ�����Ҫ����һά����ʽ����
	}
	if (matrix == NULL) {
		list_matrix->matrix = end_matirx;
	}
	else {
		while (matrix->next != NULL) {
			matrix = matrix->next;
		}
		matrix->next = end_matirx;
		if (list_matrix->width == 1) {
			list_matrix->height++;
		}
		else {
			list_matrix->width++;
		}	
	}
	return true;
}

//�����������з���ָ��λ�þ���
//����
//list_matrix����������
//index��λ��
//���� ����ָ��
user_nn_matrix *user_nn_matrices_ext_matrix_index(user_nn_list_matrix *list_matrix, int index){
	user_nn_matrix *matrix = list_matrix->matrix;//��ȡ��һ���������

	if (index >= (list_matrix->width * list_matrix->height) || (index < 0)){
		return NULL;
	}
	while (index--){
		matrix = matrix->next;
	}
	return matrix;
}
//�����������з���ָ��λ�þ���
//����
//list_matrix����������
//postx��x���� <=list_matrix->width
//posty��y���� <=list_matrix->height
//���� ����ָ��
user_nn_matrix *user_nn_matrices_ext_matrix(user_nn_list_matrix *list_matrix, int postx, int posty){
	return user_nn_matrices_ext_matrix_index(list_matrix,postx + posty * list_matrix->width);
}

//��ȡһ�����������0-1.0֮��
float user_nn_rand_0_1(void){
	//srand((unsigned)time(NULL));
	return (float)(rand() / double(RAND_MAX));
}
//���ݹ�һ������ �������� -1 - +1 ��Χ
float user_nn_rand__1_1(float post){
	//return 1;
	return (float)((user_nn_rand_0_1() - 0.5) * 2 * sqrt(6 / post));
}
//���õ������������ֵ �������
void user_nn_matrix_rand_vaule(user_nn_matrix *src_matrix, float post){
	int total = src_matrix->height * src_matrix->width;//���ø߶������
	float *data = src_matrix->data;//��ȡ�ڴ�ָ��
	while (total--){
		*data++ = user_nn_rand__1_1(post);//����ֵ
	}
}
//�����������������ֵ �������
void user_nn_matrices_rand_vaule(user_nn_list_matrix *list_matrix, float post){
	user_nn_matrix *matrix = list_matrix->matrix;//��ȡ��һ���������
	float *data = 0;
	int total = 0;

	if (matrix == NULL){
		return;
	}
	while (matrix != NULL){
		user_nn_matrix_rand_vaule(matrix, post);
		matrix = matrix->next;//����������һ������
	}
}
//����ָ���ϡ��¡��������䲢�����µľ��󣬲��ҿ������ݵ��µľ�����
//����
//src_matrix ��ԭ����
//above�������
//below���±߽�
//left����߽�
//right���ұ߽�
//���� �µľ���
user_nn_matrix *user_nn_matrix_expand(user_nn_matrix *src_matrix, int above, int below, int left, int right){
	user_nn_matrix *result = NULL;//
	float *src_data = src_matrix->data;//����ָ��
	float *result_data;
	int count;//
	int index = 0;

	result = user_nn_matrix_create(left + src_matrix->width + right, above + src_matrix->height + below);//��������
	result_data = result->data;//ȡ������ָ��

	result_data = result_data + (result->width * above + left);//������ͷλ��
	//��������
	for (count = 0; count < (src_matrix->width * src_matrix->height); count++){
		*result_data++ = *src_data++;
		if (++index >= src_matrix->width){
			index = 0;
			result_data = result_data + left + right;
		}
	}
	return result;
}
//
//��src_matrix����ָ��(x,y)λ�ñ���save_dataָ����������д�СΪwidth*height������ʧ�ܻ�ɹ�
//����
//src_matrix���������
//save_data����Ҫ����ľ���
//x: ��ʼ�� <=src_matrix->width
//y����ʼ�� <=src_matrix->height
//���أ��ɹ�����ʧ��
bool user_nn_matrix_save_array(user_nn_matrix *dest_matrix, float *src_data, int startx, int starty, int width, int height) {
	user_nn_matrix *result = NULL;
	int post_x, post_y;
	float *dest_data = dest_matrix->data;//����ָ��

	if (((startx + width) > dest_matrix->width) || ((starty + height) > dest_matrix->height) || (width == 0) || (height == 0)) {
		return false;//���������Χ��ôֱ�ӷ��ؿ�
	}
	dest_data += starty * dest_matrix->width + startx;//��ת����ʼλ��
	for (post_y = 0; post_y < height; post_y++) {
		for (post_x = 0; post_x < width; post_x++) {
			*dest_data++ = *src_data++;
		}
		dest_data += dest_matrix->width - width;
	}
	return true;
}
//
//��src_matrix����ָ��(x,y)λ�õ���save_data*alphaָ����������д�СΪwidth*height������ʧ�ܻ�ɹ�
//����
//src_matrix���������
//save_data����Ҫ����ľ���
//alpha:ϵ��
//x: ��ʼ�� <=src_matrix->width
//y����ʼ�� <=src_matrix->height
//���أ��ɹ�����ʧ��
bool user_nn_matrix_sum_array_mult_alpha(user_nn_matrix *dest_matrix, float *src_data, float alpha,int startx, int starty, int width, int height) {
	user_nn_matrix *result = NULL;
	int post_x, post_y;
	float *dest_data = dest_matrix->data;//����ָ��

	if (((startx + width) > dest_matrix->width) || ((starty + height) > dest_matrix->height) || (width == 0) || (height == 0)) {
		return false;//���������Χ��ôֱ�ӷ��ؿ�
	}
	dest_data += starty * dest_matrix->width + startx;//��ת����ʼλ��
	for (post_y = 0; post_y < height; post_y++) {
		for (post_x = 0; post_x < width; post_x++) {
			*dest_data++ += *src_data++ * alpha;
		}
		dest_data += dest_matrix->width - width;
	}
	return true;
}
//
//��src_matrix����ָ��(x,y)λ�ñ���ֵ������ʧ�ܻ�ɹ�
//����
//src_matrix���������
//startx: ��ʼ�� <=src_matrix->width
//starty����ʼ�� <=src_matrix->height
//vaule����Ҫ���������
//���أ��ɹ�����ʧ��
bool user_nn_matrix_save_float(user_nn_matrix *src_matrix, int startx, int starty, float vaule) {

	if ((startx >= src_matrix->width) || (starty >= src_matrix->height)) {
		return false;//���������Χ��ôֱ�ӷ��ؿ�
	}
	src_matrix->data[starty * src_matrix->width + startx] = vaule;

	return true;
}
//
//��src_matrix����ָ��(x,y)λ�ñ���save_matrix���󣬷���ʧ�ܻ�ɹ�
//����
//src_matrix���������
//save_matrix����Ҫ����ľ���
//x: ��ʼ�� <=src_matrix->width
//y����ʼ�� <=src_matrix->height
//���أ��ɹ�����ʧ��
bool user_nn_matrix_save_matrix(user_nn_matrix *src_matrix, user_nn_matrix *save_matrix, int startx, int starty) {
	user_nn_matrix *result = NULL;
	int post_x, post_y;
	float *src_data = src_matrix->data;//����ָ��
	float *save_data = save_matrix->data;

	if (((startx + save_matrix->width) > src_matrix->width) || ((starty + save_matrix->height) > src_matrix->height) || (save_matrix->width == 0) || (save_matrix->height == 0)) {
		return false;//���������Χ��ôֱ�ӷ��ؿ�
	}
	src_data += starty * src_matrix->width + startx;//��ת����ʼλ��
	for (post_y = 0; post_y < save_matrix->height; post_y++) {
		for (post_x = 0; post_x < save_matrix->width; post_x++) {
			*src_data++ = *save_data++;
		}
		src_data += src_matrix->width - save_matrix->width;
	}
	return true;
}

//
//�Ӿ���ָ��(x,y)λ�ý�ȡ(w,h)��С�ľ��󡣲��ҷ����½�ȡ�ľ���
//����
//src_matrix���������
//x: ��ʼ�� <=src_matrix->width
//y����ʼ�� <=src_matrix->height
//w���᷶Χ <=src_matrix->width
//h���ݷ�Χ <=src_matrix->height
//���أ��ɹ�����ʧ��
user_nn_matrix *user_nn_matrix_ext_matrix(user_nn_matrix *src_matrix, int startx, int starty, int width, int height){
	user_nn_matrix *result = NULL;
	int post_x, post_y, post_index;
	float *src_data = src_matrix->data;//����ָ��
	float *result_data;

	if (((startx + width) > src_matrix->width) || ((starty + height) > src_matrix->height) || (width == 0) || (height == 0)){
		return NULL;//���������Χ��ôֱ�ӷ��ؿ�
	}
	result = user_nn_matrix_create(width, height);//��������
	result_data = result->data;//ȡ������ָ��

	//post_index = startx + starty* src_matrix->width;//ָ��ͨ��(postx,posty)ת��һά�����λ�� ��ʽ��index=������+������*�������
	for (post_y = 0; post_y < height; post_y++){
		for (post_x = 0; post_x < width; post_x++){
			post_index = (startx + post_x) + (starty + post_y)* src_matrix->width;//ָ��ͨ��(postx,posty)ת��һά�����λ�� ��ʽ��index=������+������*�������
			*result_data++ = src_data[post_index];//��ȡ����
			//printf("x:%d,y:%d,%d ", startx+i, starty+j, post_index);
		}
		//printf("\n");
	}
	return result;
}
//�������ľ�������ݿ�����һ��������
//����
//src_matrix����ת������
//���� ��
void user_nn_matrices_to_matrix(user_nn_matrix *src_matrix, user_nn_list_matrix *sub_matrices){
	user_nn_matrix *sub_matrix = sub_matrices->matrix;

	int count_matrix, count_data;//
	float *src_data = src_matrix->data;//ָ���������
	float *sub_data = NULL;

	//result_data = result->data;//��ȡ����ָ��

	for (count_matrix = 0; count_matrix < (sub_matrices->width * sub_matrices->height); count_matrix++){
		//user_nn_matrix_exc_width_height(sub_matrix);//�������� matlabͬ��
		sub_data = sub_matrix->data;//��ȡ����ָ��
		for (count_data = 0; count_data < (sub_matrix->width * sub_matrix->height); count_data++){
			*src_data++ = *sub_data++;//��������
		}
		sub_matrix = sub_matrix->next;
	}

}
//����һ�������ľ�������һ������������
void user_nn_matrices_cpy_matrices(user_nn_list_matrix *src_matrices, user_nn_list_matrix *dest_matrices){
	int count = dest_matrices->width * dest_matrices->height;
	user_nn_matrix *src_m = src_matrices->matrix;
	user_nn_matrix *dest_m = dest_matrices->matrix;

	while (count--){
		user_nn_matrix_cpy_matrix(src_m, dest_m);
		src_m  = src_m->next;
		dest_m = dest_m->next;
	}
}
//
//����src_matrix����(x,y)����СΪwidth*height��������dest_data,���У�����ʧ�ܻ�ɹ�
//����
//dest_data����Ҫ����ľ���
//src_matrix���������
//x: ��ʼ�� <=src_matrix->width
//y����ʼ�� <=src_matrix->height
//���أ��ɹ�����ʧ��
bool user_nn_matrix_cpy_array(float *dest_data, user_nn_matrix *src_matrix, int startx, int starty, int width, int height){
	user_nn_matrix *result = NULL;
	int post_x, post_y;
	float *src_data = src_matrix->data;//����ָ��

	if (((startx + width) > src_matrix->width) || ((starty + height) > src_matrix->height) || (width == 0) || (height == 0)) {
		return false;//���������Χ��ôֱ�ӷ��ؿ�
	}
	src_data += starty * src_matrix->width + startx;//��ת����ʼλ��
	for (post_y = 0; post_y < height; post_y++) {
		for (post_x = 0; post_x < width; post_x++) {
			*dest_data++ = *src_data++;
		}
		src_data += src_matrix->width - width;
	}
	return true;
}
//
//����src_matrix*constant����(x,y)����СΪwidth*height��������dest_data,���У�����ʧ�ܻ�ɹ�
//����
//dest_data����Ҫ����ľ���
//src_matrix���������
//x: ��ʼ�� <=src_matrix->width
//y����ʼ�� <=src_matrix->height
//���أ��ɹ�����ʧ��
bool user_nn_matrix_cpy_array_mult_constant(float *dest_data, user_nn_matrix *src_matrix, int startx, int starty, int width, int height, float constant) {
	user_nn_matrix *result = NULL;
	int post_x, post_y;
	float *src_data = src_matrix->data;//����ָ��

	if (((startx + width) > src_matrix->width) || ((starty + height) > src_matrix->height) || (width == 0) || (height == 0)) {
		return false;//���������Χ��ôֱ�ӷ��ؿ�
	}
	src_data += starty * src_matrix->width + startx;//��ת����ʼλ��
	for (post_y = 0; post_y < height; post_y++) {
		for (post_x = 0; post_x < width; post_x++) {
			*dest_data++ = *src_data++ * constant;
		}
		src_data += src_matrix->width - width;
	}
	return true;
}
//��һ������ת��Ϊ�����ľ�������
//src_matrix���������
//width��Ŀ�����
//height��Ŀ��߶�
//���� ��������
void user_nn_matrix_to_matrices(user_nn_list_matrix *src_matrices, user_nn_matrix *sub_matrix){
	user_nn_matrix *matrix = src_matrices->matrix;
	int count_matrix, count_data;//
	float *result_data = sub_matrix->data;//ָ���������
	float *src_data = NULL;

	for (count_matrix = 0; count_matrix < (src_matrices->width * src_matrices->height); count_matrix++){
		src_data = matrix->data;//��ȡ��������ָ��
		for (count_data = 0; count_data < (matrix->width * matrix->height); count_data++){
			*src_data++ = *result_data++;//��������
		}
		//user_nn_matrix_exc_width_height(matrix);//�������� matlabͬ��
		matrix = matrix->next;
	}

}
//�������� ��ֵ����
//src_matrix��
//width�����䱶��
//height�����䱶��
//���� �µľ���
user_nn_matrix *user_nn_matrix_expand_mult_constant(user_nn_matrix *src_matrix, int width, int height, float constant){
	user_nn_matrix * result = NULL;
	float *src_data = src_matrix->data;
	float *result_data = NULL;
	int x, y,wi,hi;

	result = user_nn_matrix_create(src_matrix->width * width, src_matrix->height * height);//���������ľ���
	result_data = result->data;

	for (y = 0; y < src_matrix->height; y++){
		for (hi = 0; hi < height; hi++){
			for (x = 0; x < src_matrix->width; x++){
				for (wi = 0; wi < width; wi++){
					*result_data++ = (float)*src_data * constant;//��������
				}
				src_data++;
			}
			src_data = src_data - src_matrix->width;//��ת����ʼλ��
		}
		src_data = src_data + src_matrix->width;//��ת������λ��
	}
	return result;
}
//���þ���ֵ
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//v�����õ�ֵ
//���� ��
void user_nn_matrix_memset(user_nn_matrix *save_matrix, float constant){
	int count = save_matrix->width * save_matrix->height;//��ȡ�������ݴ�С
	float *src_data = save_matrix->data;
	while (count--){
		*src_data++ = constant;
	}
}
//���þ���ֵ
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//data������ָ�� ���ھ���
//���� ��
void user_nn_matrix_memcpy(user_nn_matrix *save_matrix, float *data){
	int count = save_matrix->width * save_matrix->height;//��ȡ�������ݴ�С
	float *src_data = save_matrix->data;
	while (count--){
		*src_data++ = *data++;
	}
}
//���þ���ֵ
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//data������ָ�� ���ھ���
//���� ��
void user_nn_matrix_memcpy_uchar_mult_constant(user_nn_matrix *save_matrix, unsigned char *input_array, float constant){
	int count = save_matrix->width * save_matrix->height;//��ȡ�������ݴ�С
	float *src_data = save_matrix->data;
	while (count--){
		*src_data++ = *input_array++ * constant;
	}
}
//����������������
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//data������ָ�� ���ھ���
//���� ��
void user_nn_matrix_uchar_memcpy(unsigned char *save_array, user_nn_matrix *src_matrix){
	int count = src_matrix->width * src_matrix->height;//��ȡ�������ݴ�С
	float *src_data = src_matrix->data;
	while (count--){
		*save_array++ = (unsigned char)*src_data++;
	}
}
//�������
//src_matrix��Ŀ����� �������Զ�ɾ��
//type����ʾ�����������
//���أ�����������
//
user_nn_matrix *user_nn_matrix_sorting(user_nn_matrix *src_matrix, sorting_type type){
	user_nn_matrix *result = NULL;
	user_nn_matrix *cpy_matrix = NULL;//��ʱ����
	int count = 0;//��ȡ�������ݴ�С
	float *post_index = NULL;
	float *result_data = NULL;

	cpy_matrix = user_nn_matrix_cpy_create(src_matrix);//����һ������
	count = cpy_matrix->width * cpy_matrix->height;//��ȡ�������ݴ�С

	result = user_nn_matrix_create(cpy_matrix->width,cpy_matrix->height);//����һ���µľ���
	result_data = result->data;//

	while(count--){
		if(type == sorting_up){
			post_index = user_nn_matrix_return_min_addr(cpy_matrix);//��ȡ��Сֵ��λ��
			*result_data++ = *post_index;//������Сֵ
			*post_index = FLT_MAX;//ɾ����Сֵ ��ֵ��󼴿�		
		}else if(type == sorting_down){
			post_index = user_nn_matrix_return_max_addr(cpy_matrix);//��ȡ��Сֵ��λ��
			*result_data++ = *post_index;//������Сֵ
			*post_index = -FLT_MAX;//ɾ�����ֵ ��ֵ��С����		
		}
	}
	user_nn_matrix_delete(cpy_matrix);

	return result;
}

//��;����볣��
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//bias��ƫ�ò���
//���� ��
void user_nn_matrix_sum_constant(user_nn_matrix *src_matrix, float constant){
	int count = src_matrix->width * src_matrix->height;//��ȡ�������ݴ�С
	float *src_data = src_matrix->data;

	while (count--){
		*src_data++ = *src_data + constant;
	}
}
//����save_matrix��;���src_matrix��alpha�ĳ˻� save_matrix=save_matrix+src_matrix*alpha
//����
//save_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//src_matrix������;���
//alpha������
//���� ��
void user_nn_matrix_sum_matrix_mult_alpha(user_nn_matrix *save_matrix, user_nn_matrix *src_matrix, float alpha){
	int count = save_matrix->width * save_matrix->height;//��ȡ�������ݴ�С
	float *save_data = save_matrix->data;
	float *src_data = src_matrix->data;

	while (count--){
		*save_data++ = *save_data + *src_data++ * alpha;
	}
}
//��;�����������
//����
//src_matrix��
//bias��ƫ�ò���
//���� ��
float user_nn_matrix_cum_element(user_nn_matrix *src_matrix){
	float result = 0;
	float *src_data = src_matrix->data;
	int count = src_matrix->width * src_matrix->height;

	while (count--){
		result += *src_data++;
	}
	return result;
}
//�����������  save_matrix = src_matrix + sub_matrix 
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//sub_matrix������;���
//���� ��
void user_nn_matrix_cum_matrix(user_nn_matrix *save_matrix, user_nn_matrix *src_matrix, user_nn_matrix *sub_matrix){
	int count = sub_matrix->width * sub_matrix->height;//��ȡ�������ݴ�С
	float *save_data = save_matrix->data;
	float *src_data = src_matrix->data;
	float *sub_data = sub_matrix->data;

	if ((src_matrix->width != sub_matrix->width) || (src_matrix->height != sub_matrix->height)){
		return;
	}
	while (count--){
		*save_data++ = *src_data++ + (*sub_data++);
	}
}
//�����������  save_matrix = src_matrix + sub_matrix * alpha
//����
//src_matrix��Ŀ����� ���ֵ�Ḳ�Ǵ˾���
//sub_matrix������;���
//���� ��
void user_nn_matrix_cum_matrix_mult_alpha(user_nn_matrix *save_matrix, user_nn_matrix *src_matrix, user_nn_matrix *sub_matrix, float alpha) {
	int count = sub_matrix->width * sub_matrix->height;//��ȡ�������ݴ�С
	float *save_data = save_matrix->data;
	float *src_data = src_matrix->data;
	float *sub_data = sub_matrix->data;

	if ((src_matrix->width != sub_matrix->width) || (src_matrix->height != sub_matrix->height)) {
		return;
	}
	while (count--) {
		*save_data++ = *src_data++ + ((*sub_data++) * alpha);
	}
}
//����sub_matrix����ֵ��src_matrix����
//���� Ҫ�������ͬ
//src_matrix������
//sub_matrix������
//���� ��
void user_nn_matrix_cpy_matrix(user_nn_matrix *save_matrix, user_nn_matrix *sub_matrix){
	int count = sub_matrix->width * sub_matrix->height;//��ȡ�������ݴ�С
	float *save_data = save_matrix->data;
	float *sub_data = sub_matrix->data;

	if ((save_matrix->width != sub_matrix->width) && (save_matrix->height != sub_matrix->height)){
		return;
	}
	while (count--){
		*save_data++ = *sub_data++;
	}
}
//����sub_matrix����ֵ��src_matrix���� �����ڸ���λ�ý�����Ͳ���
//���� Ҫ�������ͬ
//src_matrix������
//sub_matrix������
//index������λ��
//constant����Ͳ���
//���� ��
void user_nn_matrix_cpy_matrix_sum_constant(user_nn_matrix *save_matrix, user_nn_matrix *sub_matrix, int index, float constant){
	int count = sub_matrix->width * sub_matrix->height;//��ȡ�������ݴ�С
	float *save_data = save_matrix->data;
	float *sub_data = sub_matrix->data;

	for (count = 0; count<(sub_matrix->width * sub_matrix->height); count++){
		if (count == index){
			*save_data++ = *sub_data++ + constant;
		}
		else{
			*save_data++ = *sub_data++;
		}
	}
}
//����һά������� ��������Ĵ�С��Ҫһ���Ҷ���һά��������
//����
//src_matrix������a
//sub_matrix������b
//���� ���
float user_nn_matrix_mult_cum_matrix(user_nn_matrix *src_matrix, user_nn_matrix *sub_matrix){
	int count = sub_matrix->width * sub_matrix->height;
	float *src_data = src_matrix->data;
	float *sub_data = sub_matrix->data;
	float result = 0;

	while (count--){
		result += (*src_data++) * (*sub_data++);
	}
	return result;
}

//����˷�
//1.������A���������ھ���B������ʱ��A��B������ˡ�
//2.����C���������ھ���A��������C����������B��������
//3.�˻�C�ĵ�m�е�n�е�Ԫ�ص��ھ���A�ĵ�m�е�Ԫ�������B�ĵ�n�ж�ӦԪ�س˻�֮�͡�
//����
//src_matrix������A
//sub_matrix������B
//����ֵ ��
user_nn_matrix *user_nn_matrix_mult_matrix(user_nn_matrix *src_matrix, user_nn_matrix *sub_matrix){
#ifdef WIN64
	return user_nn_matrix_mult_matrix_cuda(src_matrix, sub_matrix);
#else

	user_nn_matrix *result = NULL;//�������
	float *src_data = NULL;//
	float *sub_data = NULL;//

	float *result_data = NULL;

	int width, height,point;//��������

	if (src_matrix->width != sub_matrix->height){//����˻�ֻ�е���һ�����������=�ڶ��������������������
		return NULL;
	}
	result = user_nn_matrix_create(sub_matrix->width, src_matrix->height);//�����µľ���
	result_data = result->data;//��ȡ����ָ��

	for (height = 0; height < result->height; height++){
		for (width = 0; width < result->width; width++){
			src_data = src_matrix->data + height * src_matrix->width;//ָ���п�ͷ
			sub_data = sub_matrix->data + width;//ָ���п�ͷ
			for (point = 0; point < sub_matrix->height; point++){
				*result_data += *src_data * *sub_data;
				sub_data += sub_matrix->width;
				src_data++;
			}
			result_data++;
		}
	}
	return result;
#endif
}
//����������е�˲��� ��Ӧ���ݽ������
//src_matrix������A
//sub_matrix������B
//����ֵ ��
void user_nn_matrix_poit_mult_matrix(user_nn_matrix *save_matrix, user_nn_matrix *src_matrix, user_nn_matrix *sub_matrix){
	int count = sub_matrix->width * sub_matrix->height;
	float *save_data = save_matrix->data;
	float *src_data = src_matrix->data;
	float *sub_data = sub_matrix->data;

	while (count--){
		*save_data++ = (*src_data++) * (*sub_data++);
	}
}
//����ÿ��Ԫ��*����
//����
//src_matrix������
//����ֵ ��
void user_nn_matrix_mult_constant(user_nn_matrix *src_matrix, float constant){
	int count = src_matrix->width * src_matrix->height;//��ȡ�������ݴ�С
	float *src_data = src_matrix->data;

	while (count--){
		*src_data++ = (float)*src_data * constant;
	}
}
//�������
//����
//src_matrix������
//����ֵ ��
void user_nn_matrix_divi_constant(user_nn_matrix *src_matrix, float constant){
	int count = src_matrix->width * src_matrix->height;//��ȡ�������ݴ�С
	float *src_data = src_matrix->data;

	while (count--){
		*src_data++ = (float)*src_data / constant;
	}
}
//��������ת180��
//����
//output�����ͼ��
//input������ͼ��
//���� �ɹ���ʧ��
//
user_nn_matrix *user_nn_matrix_rotate180(user_nn_matrix *src_matrix){
	user_nn_matrix *result = NULL;
	int count = src_matrix->width * src_matrix->height;
	float *input_data = src_matrix->data;
	float *result_data;

	result = user_nn_matrix_create(src_matrix->width, src_matrix->height);
	result_data = result->data;//ȡ������ָ��

	while (count--){
		*result_data++ = input_data[count];//ֱ����β���н���
	}
	return result;
}
//�Ծ������pooling���� �˲������cnnʹ��
//����
//save_matrix:�ػ���ľ������
//src_matrix���ػ�����
//kernel_matrix���ػ������С
//���� �ػ���ľ���
void user_nn_matrix_pooling(user_nn_matrix *save_matrix, user_nn_matrix *src_matrix, user_nn_matrix *kernel_matrix){
	user_nn_matrix *temp_matrix = NULL;//����һ����ʱ���汻�������ݵľ���

	int start_x, start_y;//���ﱣ�濪ʼ��x��yλ��
	float *save_data = save_matrix->data;//�������ݵ�ָ��

	for (start_y = 0; start_y < (src_matrix->height / kernel_matrix->height); start_y++){//�����ƶ�һ�� ������Ҫ�ƶ�������
		for (start_x = 0; start_x < (src_matrix->width / kernel_matrix->width); start_x++){
			temp_matrix = user_nn_matrix_ext_matrix(src_matrix, start_x * kernel_matrix->width, start_y * kernel_matrix->height, kernel_matrix->width, kernel_matrix->height);//�ӱ����������л�ȡ�������� ���ݴ�СΪģ���С
			*save_data++ = user_nn_matrix_mult_cum_matrix(temp_matrix, kernel_matrix);//�˻��ۼ�
			user_nn_matrix_delete(temp_matrix);//ɾ������
		}
	}

}

//�Ծ�����о�������
//����
//src_matrix����������
//kernel_matrix��������
//type���������� full same valid ��
//���� ��
user_nn_matrix *user_nn_matrix_conv2(user_nn_matrix *src_matrix, user_nn_matrix *kernel_matrix, user_nn_conv2_type type){
	user_nn_matrix *conv_matrix = NULL;//����������
	user_nn_matrix *mode_matrix = NULL;//����һ����ʱ��������˴�С�ľ��� ���ھ���ת180��
	user_nn_matrix *temp_matrix = NULL;//����
	user_nn_matrix *full_matrix = NULL;//����
	user_nn_matrix *same_matrix = NULL;//����
	user_nn_matrix *result = NULL;//�������
	
	int start_x, start_y;//���ﱣ�濪ʼ��x��yλ��
	float *result_data = NULL;

	if (type == u_nn_conv2_type_valid){
		result = user_nn_matrix_create(src_matrix->width - kernel_matrix->width + 1, src_matrix->height - kernel_matrix->height + 1);//����һ���û����ؽ���ľ���
		result_data = result->data;//ָ��������������ָ��
		conv_matrix = src_matrix;//��������ָ��
	}else if (type == u_nn_conv2_type_full){
		full_matrix = user_nn_matrix_expand(src_matrix, kernel_matrix->height - 1, kernel_matrix->height - 1, kernel_matrix->width - 1, kernel_matrix->width - 1);//�߽���չ
		result = user_nn_matrix_create(full_matrix->width - kernel_matrix->width + 1, full_matrix->height - kernel_matrix->height + 1);//����һ���û����ؽ���ľ���
		result_data = result->data;//ָ��������������ָ��
		conv_matrix = full_matrix;//��������ָ��
	}
	else if (type == u_nn_conv2_type_same){
		same_matrix = user_nn_matrix_expand(src_matrix, (kernel_matrix->height - 1) / 2, (kernel_matrix->height) / 2, (kernel_matrix->width - 1) / 2, (kernel_matrix->width) / 2);//������� ����һ���µľ���
		result = user_nn_matrix_create(same_matrix->width - kernel_matrix->width + 1, same_matrix->height - kernel_matrix->height + 1);//����һ���û����ؽ���ľ���
		result_data = result->data;//ָ��������������ָ��
		conv_matrix = same_matrix;//��������ָ��
	}
	else{}
	mode_matrix = user_nn_matrix_rotate180(kernel_matrix);//ģ�巭ת180��
	for (start_y = 0; start_y < result->height; start_y++){//�����ƶ�һ�� ������Ҫ�ƶ�������
		for (start_x = 0; start_x < result->width; start_x++){
			temp_matrix = user_nn_matrix_ext_matrix(conv_matrix, start_x, start_y, kernel_matrix->width, kernel_matrix->height);//�ӱ����������л�ȡ�������� ���ݴ�СΪģ���С
			*result_data++ = user_nn_matrix_mult_cum_matrix(temp_matrix, mode_matrix);//�������� 
			user_nn_matrix_delete(temp_matrix);//ɾ������
		}
	}
	user_nn_matrix_delete(mode_matrix);
	user_nn_matrix_delete(full_matrix);//ɾ������
	return result;
}

//�������ľ������
//src_matrix��������ľ���
//���� ��ʧ����
float user_nn_matrix_get_rms(user_nn_matrix *src_matrix) {
	//user_nn_matrix_poit_mult_matrix(error_matrix_temp, error_matrix_temp, error_matrix_temp);//����˷�
	//*loss_vaule = *loss_vaule + user_nn_matrix_cum_element(error_matrix_temp) / (error_matrix_temp->height*error_matrix_temp->width);//������ʧ����
	float loss = 0.0f;
	float *src_data = src_matrix->data;
	int count = src_matrix->width * src_matrix->height;
	while (count--) {
		loss = *src_data * *src_data;
		src_data++;
	}
	return float(loss / (src_matrix->width*src_matrix->height));
}

//������
//dest��ԭ����
//m�����ƴ�ֱ���и���
//n������ˮƽ���и���
//���� �µľ���
user_nn_matrix *user_nn_matrix_repmat(user_nn_matrix *dest, int m, int n) {
	user_nn_matrix *result = user_nn_matrix_create(dest->width * n, dest->height * m);
	int loca_x = 0;
	int loca_y = 0;

	for (loca_y = 0; loca_y < m; loca_y++) {
		for (loca_x = 0; loca_x < n; loca_x++) {
			user_nn_matrix_save_array(result, dest->data, loca_x*dest->width, loca_y*dest->height, dest->width, dest->height);
		}
	}
	return result;
}
//��һ������Խ�������Ϊ1 Ŀǰ����֧�ַ���
//dest��Ŀ�����
//���� �������
void user_nn_matrix_eye(user_nn_matrix *dest) {
	int count = 0;
	for (count = 0; count < dest->width; count++) {
		*user_nn_matrix_ext_value(dest, count, count) = 1.0f;
	}
}

//���givens ��ת���ֵ
//x:xֵ
//y:yֵ
//�ο�matlab���б�д
//���ؾ���
user_nn_matrix *user_nn_givens(float x, float y) {
	user_nn_matrix *result = user_nn_matrix_create(2, 2);//��������2x2
	float c = 0.0f;
	float s = 0.0f;
	float nrm = 0.0f;
	float absx = 0.0f;

	absx = abs(x);
	if (absx == 0.0f) {
		c = 0;
		s = 1;
	}
	else {
		/*
		//matlab
		nrm = (float)hypot(x,y);
		c = absx / nrm;
		s = (float)(x / absx)*(y/ nrm);
		*/
		//ά���ٿ�
		nrm = (float)hypot(x, y);
		c = x / nrm;
		s = y / nrm;
	}
	result->data[0] = c; result->data[1] = s;
	result->data[2] = -s; result->data[3] = c;

	return result;
}
//householder reflection��ʽ�������QRֵ
//dest�����ݾ���
//coordinate����������
//�ο���https://en.wikipedia.org/wiki/Householder_transformation
//���� QR����
user_nn_list_matrix *user_nn_householder_qr(user_nn_matrix *dest) {

	user_nn_list_matrix *result = user_nn_matrices_create(2, 1, dest->width, dest->height);//�����������������һ������Q �ڶ�������R
	user_nn_matrix *matrix_G = user_nn_matrix_create(dest->width, dest->width);//��������
	user_nn_matrix *matrix_Q = result->matrix;//��һ�����󱣴�Q
	user_nn_matrix *matrix_R = result->matrix->next;//�ڶ������󱣴�R
	user_nn_matrix *matrix_m = NULL;
	user_nn_matrix *matrix_e = NULL;
	user_nn_matrix *matrix_c = NULL;
	user_nn_matrix *matrix_temp = NULL;

	int index = 0;
	float norm_m = 0.0f;

	user_nn_matrix_cpy_matrix(matrix_R, dest);//������matrix_R��
	user_nn_matrix_eye(matrix_Q);//����Q�ĶԽ���Ϊ1

	for (index = 0; index < dest->height - 1; index++) {
		user_nn_matrix_memset(matrix_G, 0);//����GΪ0
		user_nn_matrix_eye(matrix_G);//����G�ĶԽ���Ϊ1

		matrix_m = user_nn_matrix_ext_matrix(matrix_R, index, index, 1, matrix_R->height - index);//��ȡ����
		matrix_e = user_nn_matrix_create(matrix_m->width, matrix_m->height);//���´�������
		matrix_e->data[0] = user_nn_matrix_norm(matrix_m);//��ȡ����
		user_nn_matrix_cum_matrix_mult_alpha(matrix_m, matrix_m, matrix_e,-1.0f);//�������֮��
		user_nn_matrix_divi_constant(matrix_m, user_nn_matrix_norm(matrix_m));//��������
		matrix_c = user_nn_matrix_outer(matrix_m, matrix_m);//������outer
		user_nn_matrix_sum_array_mult_alpha(matrix_G, matrix_c->data,-2.0f, index, index, matrix_c->width, matrix_c->height);//��������
		matrix_temp = user_nn_matrix_mult_matrix(matrix_G, matrix_R);//����˷�
		user_nn_matrix_cpy_matrix(matrix_R, matrix_temp);//������matrix_R��
		user_nn_matrix_delete(matrix_temp);//ɾ������
		//Q=G1*G2*..Gn  Q������������Q
		matrix_temp = user_nn_matrix_mult_matrix(matrix_Q, matrix_G);//����˷�
		user_nn_matrix_cpy_matrix(matrix_Q, matrix_temp);//������matrix_Q��
		user_nn_matrix_delete(matrix_temp);//ɾ������

		user_nn_matrix_delete(matrix_m);//ɾ������
		user_nn_matrix_delete(matrix_e);//ɾ������
		user_nn_matrix_delete(matrix_c);//ɾ������
	}
	user_nn_matrix_delete(matrix_G);//ɾ������
	return result;
}
//givens rotation��ʽ�������QRֵ
//dest�����ݾ���
//coordinate����������
//�ο���https://en.wikipedia.org/wiki/Givens_rotation
//���� QR����
user_nn_list_matrix *user_nn_givens_qr(user_nn_matrix *dest) {
	user_nn_list_matrix *result = user_nn_matrices_create(2, 1, dest->width, dest->height);//�����������������һ������Q �ڶ�������R
	user_nn_matrix *matrix_G = user_nn_matrix_create(dest->width, dest->height);//��������
	user_nn_matrix *matrix_Q = result->matrix;//��һ�����󱣴�Q
	user_nn_matrix *matrix_R = result->matrix->next;//�ڶ������󱣴�R
	user_nn_matrix *matrix_temp = NULL;//��ʱ����
	user_nn_matrix *triangle_axis = NULL;//��������������
	user_nn_matrix *givens_vaule = NULL;

	int posit_x = 0;
	float *axis_x = NULL;
	float *axis_y = NULL;
	float givens_r = 0.0f;
	float givens_c = 0.0f;
	float givens_s = 0.0f;

	user_nn_matrix_cpy_matrix(matrix_R, dest);//������matrix_R��
	user_nn_matrix_eye(matrix_Q);//����Q�ĶԽ���Ϊ1

	triangle_axis = user_nn_tril_indices(dest->width, dest->height, 0.0f);//��ȡ��������
	axis_x = triangle_axis->data;//��ȡX����
	axis_y = triangle_axis->data + triangle_axis->width;//��ȡY����
	for (posit_x = 0; posit_x < triangle_axis->width; posit_x++) {	//ѭ����������
		if (*user_nn_matrix_ext_value(matrix_R, (int)*axis_x, (int)*axis_y) != 0) {//�ж�Ŀ�����������Ƿ�Ϊ0 �����0��ô���ü���
			user_nn_matrix_memset(matrix_G, 0);//����GΪ0
			user_nn_matrix_eye(matrix_G);//����G�ĶԽ���Ϊ1

			givens_vaule = user_nn_givens(*user_nn_matrix_ext_value(matrix_R, (int)*axis_x, (int)*axis_x), *user_nn_matrix_ext_value(matrix_R, (int)*axis_x, (int)*axis_y));//���givens��ת���ֵ
			*user_nn_matrix_ext_value(matrix_G, (int)*axis_x, (int)*axis_x) = givens_vaule->data[0];
			*user_nn_matrix_ext_value(matrix_G, (int)*axis_y, (int)*axis_x) = givens_vaule->data[1];
			*user_nn_matrix_ext_value(matrix_G, (int)*axis_x, (int)*axis_y) = givens_vaule->data[2];
			*user_nn_matrix_ext_value(matrix_G, (int)*axis_y, (int)*axis_y) = givens_vaule->data[3];
			user_nn_matrix_delete(givens_vaule);//ɾ������
												//G1*A1=A2 G2*A2=A3 ... Gn-1*An-1=An  An������������R
			matrix_temp = user_nn_matrix_mult_matrix(matrix_G, matrix_R);//����˷�
			user_nn_matrix_cpy_matrix(matrix_R, matrix_temp);//������matrix_R��
			user_nn_matrix_delete(matrix_temp);//ɾ������
											   //Q=G1*G2*..Gn  Q������������Q
			user_nn_matrix_transpose(matrix_G);//����ת��
			matrix_temp = user_nn_matrix_mult_matrix(matrix_Q, matrix_G);//����˷�
			user_nn_matrix_cpy_matrix(matrix_Q, matrix_temp);//������matrix_Q��
			user_nn_matrix_delete(matrix_temp);//ɾ������
		}
		axis_x++;
		axis_y++;
	}
	user_nn_matrix_delete(matrix_G);//ɾ������
	user_nn_matrix_delete(triangle_axis);//ɾ������

	return result;
}
//��Ŀ�������ȡ������������ֵ
//dest��Ŀ�����
//coordinate����������
//iter����������
//���أ�����ֵ����������
user_nn_list_matrix *user_nn_eigs(user_nn_matrix *dest, float epsilon, eigs_type type) {
	user_nn_list_matrix *result = user_nn_matrices_create(2, 1, dest->width, dest->height);//�����������������һ������LATENT �ڶ�������COEFF
	user_nn_list_matrix *QR_list = NULL;
	user_nn_matrix *matrix_latent = NULL;
	user_nn_matrix *matrix_coeff = NULL;
	user_nn_matrix *matrix_tmp = NULL;
	float n_latent_trace = 0.0f;//���浱ǰ����ֵ��
	float o_latent_trace = 0.0f;//������ʷ����ֵ��

	matrix_latent = result->matrix;//LATENT matrix_latent
	matrix_coeff = result->matrix->next;//COEFF matrix_coeff

	user_nn_matrix_cpy_matrix(matrix_latent, dest);//��������
	user_nn_matrix_eye(matrix_coeff);//����G�ĶԽ���Ϊ1
	for (;;) {
		if (type == qr_givens) {
			QR_list = user_nn_givens_qr(matrix_latent);//����givens����һ��QR
		}
		else if(type == qr_householder){
			QR_list = user_nn_householder_qr(matrix_latent);//����householder����һ��QRֵ
		}
		matrix_tmp = user_nn_matrix_mult_matrix(QR_list->matrix->next, QR_list->matrix);//������������Ҫ�����µľ���
		user_nn_matrix_cpy_matrix(matrix_latent, matrix_tmp);//��������
		user_nn_matrix_delete(matrix_tmp);
		//
		matrix_tmp = user_nn_matrix_mult_matrix(matrix_coeff, QR_list->matrix);//��ȡ����ֵ
		user_nn_matrix_cpy_matrix(matrix_coeff, matrix_tmp);//��������
		user_nn_matrix_delete(matrix_tmp);

		user_nn_matrices_delete(QR_list);//ɾ����������

		//�Խ�������ֵ���ڱ仯��ô��������
		n_latent_trace = user_nn_matrix_trace(matrix_latent);//�������ֵ�ĶԽ��ߺ�
		if (abs(n_latent_trace - o_latent_trace) <= epsilon) {
			break;
		}
		else {
			o_latent_trace = n_latent_trace;
		}
	}

	return result;
}
//�������ƽ��ֵ
//src_matrix��ԭʼ����
//����ƽ��ֵ����
user_nn_matrix *user_nn_matrix_mean(user_nn_matrix *src_matrix) {
	int height_index = 0;
	user_nn_matrix *matrix_mean = user_nn_matrix_create(src_matrix->width, 1);//����һ������ƽ��ֵ�ľ���
	user_nn_matrix *matrix_temp = user_nn_matrix_create(src_matrix->width, 1);//

	for (height_index = 0; height_index < src_matrix->height; height_index++) {
		user_nn_matrix_cpy_array(matrix_temp->data, src_matrix, 0, height_index, matrix_temp->width, matrix_temp->height);//����һ������
		user_nn_matrix_sum_matrix_mult_alpha(matrix_mean, matrix_temp, 1.0f);//��;���
	}
	user_nn_matrix_divi_constant(matrix_mean, (float)src_matrix->height);//��ƽ����
	user_nn_matrix_delete(matrix_temp);//ɾ������

	return matrix_mean;
}
//���Э�������
//src_matrix����Ҫ���ľ���
//��������Ľ��
user_nn_matrix *user_nn_matrix_cov(user_nn_matrix *src_matrix) {
	user_nn_matrix *result = NULL;
	user_nn_matrix *src_matrix_s = NULL;//��ʱ����
	user_nn_matrix *src_matrix_t = NULL;//ת�þ���
	user_nn_matrix *matrix_mean = user_nn_matrix_create(src_matrix->width, 1);//����һ������ƽ��ֵ�ľ���

	int height_index = 0;
	matrix_mean = user_nn_matrix_mean(src_matrix);//��ȡƽ��ֵ
	src_matrix_s = user_nn_matrix_cpy_create(src_matrix);//���ƾ���

	for (height_index = 0; height_index < src_matrix_s->height; height_index++) {
		user_nn_matrix_sum_array_mult_alpha(src_matrix_s, matrix_mean->data, -1.0f, 0, height_index, matrix_mean->width, matrix_mean->height);//��ȥƽ��ֵ
	}
	src_matrix_t = user_nn_matrix_cpy_create(src_matrix_s);//��������ת�þ���AT�ľ���
	user_nn_matrix_transpose(src_matrix_t);//����ת��
	result = user_nn_matrix_mult_matrix(src_matrix_t, src_matrix_s);//�������
	user_nn_matrix_divi_constant(result, (float)(result->width - 1));//����n-1

	user_nn_matrix_delete(matrix_mean);//ɾ������
	user_nn_matrix_delete(src_matrix_s);//ɾ������
	user_nn_matrix_delete(src_matrix_t);//ɾ������

	return result;
}

//��ȡ��������Ǿ������� �������½��������겻��������
//width���������
//height������߶�
//details����ת�Ƕ�
//���� �������
user_nn_matrix *user_nn_tril_indices(int width, int height, float details) {
	user_nn_matrix *result = NULL;
	float *result_x = NULL;
	float *result_y = NULL;
	float posit_m = 0.0f;
	int posit_x = 0;
	int posit_y = 0;

	result = user_nn_matrix_create((width*(width - 1) / 2), 2);//�Ȳ�������͵õ��ܹ����ٸ�x�����
	result_x = result->data;
	result_y = result->data + result->width;
	for (posit_y = 0; posit_y < height; posit_y++) {
		posit_m = (float)(posit_y * width) / height;//y=u*xֱ�ߺ���
		if (modf(posit_m, &posit_m) > 0.0f) {
			posit_m += 1;
			if (posit_m >= width) { posit_m -= 1; }//���ܳ������ֵ����
		}
		for (posit_x = 0; posit_x < posit_m; posit_x++) {
			//printf("\n(%d,%d)", post_x, post_y);//ȡб���²������Ǿ���
			*result_y++ = (float)posit_y;
			*result_x++ = (float)posit_x;
		}
	}
	return result;
}
//��ͶԽ���Ԫ��
//src_matrix:���� �����Ƿ���
//�������ֵ
float user_nn_matrix_trace(user_nn_matrix *src_matrix) {
	int count = 0;
	float result = 0.0f;

	for (count = 0; count < src_matrix->width; count++) {
		result += *user_nn_matrix_ext_value(src_matrix, count, count);
	}
	return result;
}
//���ضԽ���Ԫ��
//src_matrix:���� �����Ƿ���
//���ؽ������
user_nn_matrix *user_nn_matrix_diag(user_nn_matrix *src_matrix) {
	user_nn_matrix *result = NULL;
	float *result_data = NULL;
	int count = 0;

	result = user_nn_matrix_create(src_matrix->width,1);
	result_data = result->data;
	for (count = 0; count < src_matrix->width; count++) {
		*result_data++ = *user_nn_matrix_ext_value(src_matrix, count, count);
	}

	return result;
}
//������ķ��� 2D��ʽ
//src_matrix:���� �����Ƿ���
//���ؽ������
float user_nn_matrix_norm(user_nn_matrix *src_matrix) {
	int count = src_matrix->width*src_matrix->height;
	float *src_matrix_data = src_matrix->data;
	float result = 0.0f;

	while (count--) {
		result += *src_matrix_data * *src_matrix_data;//ƽ���Ϳ�����
		src_matrix_data++;
	}
	result = sqrt(result);

	return result;
}
//�������������outerֵ
//src_matrix:����A 
//sub_matrix:����B
//���ؽ������
user_nn_matrix *user_nn_matrix_outer(user_nn_matrix *src_matrix, user_nn_matrix *sub_matrix) {
	user_nn_matrix *result = NULL;
	float *src_matrix_data = NULL;
	float *sub_matrix_data = NULL;
	float *result_data = NULL;
	int height = 0;
	int width = 0;

	result = user_nn_matrix_create(sub_matrix->width*sub_matrix->height,src_matrix->width*src_matrix->height);
	result_data = result->data;

	src_matrix_data = src_matrix->data;
	for (height = 0; height < src_matrix->width*src_matrix->height; height++) {
		sub_matrix_data = sub_matrix->data;
		for (width = 0; width < sub_matrix->width*sub_matrix->height; width++) {
			*result_data++ = *src_matrix_data * *sub_matrix_data++;
		}
		src_matrix_data++;
	}

	return result;
}
//ͨ���ԽǾ����Ԫ�������������µľ���
//src_matrix������
//diag�������б�
//epsilon������ֵ
//���� �µ�ֵ����ֵ
user_nn_matrix *user_nn_matrix_cut_vector(user_nn_matrix *src_matrix, user_nn_matrix *diag_matrix, float epsilon) {
	user_nn_matrix *result = NULL;
	
	float total_diag_vaule = 0.0f;
	float target_diag_vaule = 0.0f;
	float *diag_vaule_data = NULL;
	int total_width = 0;
		
	total_diag_vaule = user_nn_matrix_cum_element(diag_matrix);
	diag_vaule_data = diag_matrix->data;

	for (;;) {//�����Ҫ������
		total_width++;
		target_diag_vaule += *diag_vaule_data++;
		if (float(target_diag_vaule / total_diag_vaule) >= epsilon) {
			break;//����
		}
	}
	result = user_nn_matrix_ext_matrix(src_matrix,0,0, total_width, src_matrix->height);
	return result;
}
//��ӡ��������
//����
//list_matrix����������
//���� ��
void user_nn_matrix_printf(FILE *debug_file, user_nn_matrix *src_matrix){
	int width, height;
	float *input_data = src_matrix->data;
	//FILE *debug_file = NULL;
	//debug_file = fopen("debug.txt", "w+");
	printf("matrix: \n        width:%d,height:%d\n\n", src_matrix->width, src_matrix->height);
	if (debug_file != NULL)
	fprintf(debug_file,"matrix: \n        width:%d,height:%d\n\n", src_matrix->width, src_matrix->height);//��������

	for (height = 0; height < src_matrix->height; height++){
		for (width = 0; width < src_matrix->width; width++){
			//printf("%-10.6f ", *input_data);
			if (*input_data == FLT_MAX) {
				printf("%s ", "   max   ");
			}
			else {
				printf("%-10.6f ", *input_data);
			}
			if (debug_file != NULL) {
				if (*input_data == FLT_MAX) {
					fprintf(debug_file, "%s", "   max   ");
				}
				else {
					fprintf(debug_file, "%-10.6f ", *input_data);
				}
			}
			
			input_data++;
		}
		printf("\n\n");
		if (debug_file != NULL)
		fprintf(debug_file, "\n");
	}
	//fclose(debug_file);
	if (debug_file != NULL)
	fflush(debug_file);
}

//��ӡ������������
//����
//list_matrix����������
//���� ��
void user_nn_matrices_printf(FILE *debug_file,char *title, user_nn_list_matrix *src_matrix){
	int width, height;
	int count = src_matrix->width * src_matrix->height;

	printf("%s matrices: \n        width:%d,height:%d\n\n\n", title, src_matrix->width, src_matrix->height);
	if (debug_file != NULL)
	fprintf(debug_file, "%s matrices: \n        width:%d,height:%d\n\n\n", title, src_matrix->width, src_matrix->height);

	for (height = 0; height < src_matrix->height; height++){
		for (width = 0; width < src_matrix->width; width++){
			user_nn_matrix_printf(debug_file, user_nn_matrices_ext_matrix(src_matrix, width, height));
		}
		printf("\n\n");
		if (debug_file != NULL)
		fprintf(debug_file,"\n\n");
	}
}


/*
	//��һ���������ݿ���������һ��������
	user_nn_matrix *matrix = NULL;
	user_nn_matrix *dest = NULL;

	matrix = user_nn_matrix_create(6, 6);//����3*3��С�Ķ�ά����
	dest   = user_nn_matrix_create(2, 2);//����2*2��С�Ķ�ά����
	user_nn_matrix_rand_vaule(matrix,1);

	user_nn_matrix_printf(NULL, matrix);//��ӡ����
	bool is_success = user_nn_matrix_save_matrix(matrix, dest, 1, 4);
	printf("\n%s\n", is_success==true?"true":"false");

	user_nn_matrix_printf(NULL, matrix);//��ӡ����

	getchar();
	return 0;

*/
/*
	//��һ���ڴ����ݿ���������һ�������� ���չ涨����
	user_nn_matrix *matrix = NULL;
	user_nn_matrix *dest = NULL;

	matrix = user_nn_matrix_create(6, 6);//����3*3��С�Ķ�ά����
	dest = user_nn_matrix_create(2, 2);//����2*2��С�Ķ�ά����
	user_nn_matrix_rand_vaule(matrix, 1);

	user_nn_matrix_printf(NULL, matrix);//��ӡ����
	bool is_success = user_nn_matrix_save_array(matrix, dest->data, 2, 3, dest->width, dest->height);
	printf("\n%s\n", is_success == true ? "true" : "false");

	user_nn_matrix_printf(NULL, matrix);//��ӡ����

	getchar();
	return 0;
*/

/* �ػ�����
	user_nn_matrix *src_matrix = NULL;
	user_nn_matrix *sub_matrix = NULL;
	user_nn_matrix *res_matrix = NULL;
	float *result = NULL;


	src_matrix = user_nn_matrix_create(28, 28);
	sub_matrix = user_nn_matrix_create(2, 2);

	user_nn_matrix_memset(src_matrix, 2.5);//���þ���ֵ
	user_nn_matrix_memset(sub_matrix, 0.25);//���þ���ֵ

	result = user_nn_matrix_ext_value(src_matrix, 27, 27);
	*result = 1;

	res_matrix = user_nn_matrix_pool(src_matrix, sub_matrix);
	if (res_matrix != NULL){
	user_nn_matrix_printf(res_matrix);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/* �����������
	user_nn_matrix *src_matrix = NULL;
	user_nn_matrix *sub_matrix = NULL;
	user_nn_matrix *res_matrix = NULL;

	src_matrix = user_nn_matrix_create(28, 28);
	sub_matrix = user_nn_matrix_create(5, 5);

	user_nn_matrix_memset(src_matrix, 2.5);//���þ���ֵ
	user_nn_matrix_memset(sub_matrix, 1.0);//���þ���ֵ

	res_matrix = user_nn_matrix_conv2(src_matrix, sub_matrix, u_nn_conv2_type_valid);
	if (res_matrix != NULL){
	user_nn_matrix_printf(res_matrix);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/* ��ά���� ��ȡ
	user_nn_list_matrix *list = NULL;
	user_nn_matrix *dest = NULL;

	list = user_nn_matrices_create(2, 2, 1, 1);//����2*2��1*1��С�Ķ�ά����
	user_nn_matrix_memset(list->matrix, 1);
	user_nn_matrix_memset(list->matrix->next, 2);
	user_nn_matrix_memset(list->matrix->next->next, 3);
	user_nn_matrix_memset(list->matrix->next->next->next, 4);

	dest = user_nn_matrices_ext_matrix(list, 1, 0);//��ȡ����һ������

	if (dest != NULL){
	user_nn_matrix_printf(NULL,dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/* �����ȡ����
	user_nn_matrix *matrix = NULL;
	user_nn_matrix *dest = NULL;

	matrix = user_nn_matrix_create(2, 2);//����2*2��С�Ķ�ά����
	matrix->data[0] = 1.0;
	matrix->data[1] = 2.0;
	matrix->data[2] = 3.0;
	matrix->data[3] = 4.0;

	dest = user_nn_matrix_ext_matrix(matrix, 1, 1, 1, 1);//��ȡ����

	if (dest != NULL){
	user_nn_matrix_printf(NULL,dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/


/*��ȡ�����е�һ��ֵ
	user_nn_matrix *matrix = NULL;
	float *p;

	matrix = user_nn_matrix_create(2, 2);//����2*2��С�Ķ�ά����
	matrix->data[0] = 1.0;
	matrix->data[1] = 2.0;
	matrix->data[2] = 3.0;
	matrix->data[3] = 4.0;

	p = user_nn_matrix_ext_value(matrix, 1, 1);//��ȡ����ֵ
	if (p != NULL)
	printf("%f \n", *p);
*/
/*ƫ�ò�������
	user_nn_list_biases *biases = NULL;
	user_nn_bias *dest = NULL;

	biases = user_nn_biases_create(4);

	biases->bias->bias = 1.0;
	biases->bias->next->bias = 2.0;
	biases->bias->next->next->bias = 3.0;
	biases->bias->next->next->next->bias = 4.0;

	dest = user_nn_biases_ext_bias(biases,0);
	if (dest != NULL)
	printf("%f \n", dest->bias);
*/
/* ����˷�
	user_nn_matrix *src_matrix = NULL;
	user_nn_matrix *sub_matrix = NULL;
	user_nn_matrix *res_matrix = NULL;

	src_matrix = user_nn_matrix_create(1, 1);
	sub_matrix = user_nn_matrix_create(1, 1);

	user_nn_matrix_memset(src_matrix, 2.5);//���þ���ֵ
	user_nn_matrix_memset(sub_matrix, 1.0);//���þ���ֵ

	res_matrix = user_nn_matrix_mult_matrix(src_matrix, sub_matrix);//�������
	if (res_matrix != NULL){
	user_nn_matrix_printf(res_matrix);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/*��������ת��Ϊ �߶�Ϊ1��������������
	user_nn_list_matrix *list = NULL;
	user_nn_matrix *dest = NULL;

	dest = user_nn_matrix_create(2,2);
	dest->data[0] = 1.0;
	dest->data[1] = 2.0;
	dest->data[2] = 3.0;
	dest->data[3] = 4.0;

	list = user_nn_matrix_to_matrices(dest,1,1);//��ȡ����һ������

	if (list != NULL){
	user_nn_matrix_printf(list->matrix);//��ӡ����
	user_nn_matrix_printf(list->matrix->next);//��ӡ����
	user_nn_matrix_printf(list->matrix->next->next);//��ӡ����
	user_nn_matrix_printf(list->matrix->next->next->next);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/*��������ת��Ϊ�߶�Ϊ1�ĵ�������
	user_nn_list_matrix *list = NULL;
	user_nn_matrix *dest = NULL;

	list = user_nn_matrices_create(5, 2, 1, 1);//����2*2��1*1��С�Ķ�ά����
	user_nn_matrix_memset(list->matrix, 1);
	user_nn_matrix_memset(list->matrix->next, 2);
	user_nn_matrix_memset(list->matrix->next->next, 3);
	user_nn_matrix_memset(list->matrix->next->next->next, 4);

	dest = user_nn_matrices_to_matrix(list);//��ȡ����һ������

	if (dest != NULL){
	user_nn_matrix_printf(NULL,dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/*����ƫ�ò���ת�������
	user_nn_list_biases *list = NULL;
	user_nn_matrix *dest = NULL;

	list = user_nn_biases_create(1);

	list->bias->bias = 1.0;
	list->bias->next->bias = 2.0;
	list->bias->next->next->bias = 3.0;
	list->bias->next->next->next->bias = 4.0;
	list->bias->next->next->next->next->bias = 5.0;

	dest = user_nn_biases_to_matrix(list);

	if (dest != NULL){
	user_nn_matrix_printf(NULL,dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/*�����ֵ����
	user_nn_matrix *matrix = NULL;
	user_nn_matrix *dest = NULL;

	matrix = user_nn_matrix_create(2, 2);//����2*2��С�Ķ�ά����
	matrix->data[0] = 1.0;
	matrix->data[1] = 2.0;
	matrix->data[2] = 3.0;
	matrix->data[3] = 4.0;

	dest = user_nn_matrix_expand_matrix(matrix, 2, 3);//

	if (dest != NULL){
	user_nn_matrix_printf(NULL,dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/*�������� �߿�����
	user_nn_matrix *matrix = NULL;
	user_nn_matrix *dest = NULL;

	matrix = user_nn_matrix_create(1, 1);//����2*2��С�Ķ�ά����
	matrix->data[0] = 1.0;
	//matrix->data[1] = 2.0;
	//matrix->data[2] = 3.0;
	//matrix->data[3] = 4.0;

	dest = user_nn_matrix_expand(matrix, 1, 1);//

	if (dest != NULL){
	user_nn_matrix_printf(NULL,dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/*�������󿽱�
	user_nn_list_matrix *list = NULL;
	user_nn_list_matrix *dest = NULL;

	list = user_nn_matrices_create(5, 2, 1, 1);//����2*2��1*1��С�Ķ�ά����
	dest = user_nn_matrices_create(5, 2, 1, 1);//����2*2��1*1��С�Ķ�ά����

	user_nn_matrix_memset(list->matrix, 1);
	user_nn_matrix_memset(list->matrix->next, 2);
	user_nn_matrix_memset(list->matrix->next->next, 3);
	user_nn_matrix_memset(list->matrix->next->next->next, 4);

	user_nn_matrices_cpy_matrices(dest, list);//��ȡ����һ������

	if (dest != NULL){
	user_nn_matrices_printf(NULL,"TEST", dest);//��ӡ����
	}
	else{
	printf("null\n");
	}
*/
/* ���󽻻�
	unsigned char src[] = { 1, 2, 3, 4, 5, 6 };
	unsigned char sub[] = { 1, 2, 3 };

	user_nn_matrix *src_matrix = NULL;
	user_nn_matrix *sub_matrix = NULL;
	user_nn_matrix *res_matrix = NULL;

	src_matrix = user_nn_matrix_create(2, 3);
	sub_matrix = user_nn_matrix_create(1, 3);

	user_nn_matrix_memcpy_char(src_matrix, src);
	user_nn_matrix_memcpy_char(sub_matrix, sub);

	user_nn_matrix_exc_width_height(src_matrix);//����output_kernel_maps �� width��height
	res_matrix = user_nn_matrix_mult_matrix(src_matrix, sub_matrix);//����feature vector delta  ######������������Ҫ�仯���ݴ�С######
	user_nn_matrix_exc_width_height(src_matrix);//����output_kernel_maps �� width��height ��������

	if (res_matrix != NULL){
	user_nn_matrix_printf(NULL,res_matrix);//��ӡ����
	}
	else{
	printf("null\n");
	}

*/
/* ��������������仯
	unsigned char src[192] ;
	int i = 0;
	for (i = 0; i < 192; i++){
	src[i] = i;
	}
	user_nn_list_matrix *list = NULL;
	user_nn_matrix *dest = NULL;
	dest = user_nn_matrix_create(1, 192);
	user_nn_matrix_memcpy_char(dest, src);
	list = user_nn_matrix_to_matrices(dest, 4, 4);//��ȡ����һ������
	dest = user_nn_matrices_to_matrix(list);
	if (dest != NULL){
	user_nn_matrix_printf(NULL, dest);//��ӡ����
	}
	else{
	printf("null\n");
	}

*/
/*���󷵻����ֵ����
	user_nn_matrix *matrix = NULL;

	matrix = user_nn_matrix_create(6, 6);//����2*2��С�Ķ�ά����
	user_nn_matrix_rand_vaule(matrix, 1);

	int max_value_index = user_nn_matrix_return_max_index(matrix);//
	int min_value_index = user_nn_matrix_return_min_index(matrix);//

	user_nn_matrix_printf(NULL,matrix);//��ӡ����

	printf("\nmax_value_index=%d,vaule=%f\n",max_value_index,*user_nn_matrix_return_max_addr(matrix));
	printf("\nmin_value_index=%d,vaule=%f\n",min_value_index,*user_nn_matrix_return_min_addr(matrix));
*/


/* ����ת��ʱ�����

	FILE *debug_file = NULL;
	debug_file = fopen("debug.txt", "w+");
	user_nn_matrix *src_matrix = NULL;
	user_nn_matrix *sub_matrix = NULL;
	user_nn_matrix *res_matrix = NULL;
	int matrix_w = 128, matrix_h = 128;
	src_matrix = user_nn_matrix_create(matrix_w, matrix_h);
	sub_matrix = user_nn_matrix_create(matrix_w, matrix_h);
	clock_t test_start_time, test_end_time;
	float *src_data = src_matrix->data;
	float *sub_data = sub_matrix->data;

	int count = 0,total_count=499;

	while (count++ < (src_matrix->width * src_matrix->height)){
	*src_data++ = count * 0.01f;
	*sub_data++ = count * 0.01f;
	}
	printf("test start:\n");
	printf("width:%d,height:%d\n", src_matrix->width ,src_matrix->height);
	user_nn_matrix_transpose_cuda(sub_matrix);//�����Կ�
	Sleep(1000);
	//user_nn_matrix_printf(debug_file, src_matrix);//��ӡ����
	while (1){
	test_start_time = clock();
	count = total_count;
	while (count--){
	//user_nn_matrix_transpose_cuda(src_matrix);//GPU����
	user_nn_matrix_transpose(src_matrix);//CPUת��
	}
	src_data = src_matrix->data;//��ȡ����
	sub_data = sub_matrix->data;//��ȡ����
	count = 0;
	while (count++ < (src_matrix->width * src_matrix->height)){
	if (*src_data++ != *sub_data++){
	break;
	}
	}
	if ((count - 1) != src_matrix->width * src_matrix->height){
	printf("count:%d ", count);
	printf("test error \n");
	break;
	}
	else{
	test_end_time = (clock() - test_start_time);//��ȡ����ʱ��
	printf("total time : %f ms\n", (float)test_end_time / total_count);
	break;
	}
	}

	//res_matrix = user_nn_matrix_mult_matrix(src_matrix, sub_matrix);//�������
	//user_nn_matrix_transpose(src_matrix);
	if (src_matrix != NULL){
	//user_nn_matrix_printf(debug_file, src_matrix);//��ӡ����
	}
	else{
	printf("null\n");
	}
	fclose(debug_file);
	getchar();
	return 1;

*/