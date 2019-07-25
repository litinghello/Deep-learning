import cv2 as opencv
import numpy as np
from mnist import MNIST
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
import rebuild_matrix as rebulid
import sort_data as sortd

def display_image(data):
    opencv.imshow("image", opencv.resize(data, None, fx=2, fy=2, interpolation=opencv.INTER_CUBIC))
    opencv.waitKey(0)
    opencv.destroyAllWindows()


def save_image(matrix):
    opencv.imwrite("./temp/test",opencv.resize(matrix, None, fx=2, fy=2, interpolation=opencv.INTER_CUBIC))


def kmeans_process(n_class,data):
    kmeans = KMeans(n_clusters=n_class,init='random',algorithm='full').fit(data)
    return kmeans.cluster_centers_

#自定义排序矩阵
def custum_sort_matrix(data):
    data_list = data.tolist()
    return np.array(sorted(data_list, key=lambda x:np.linalg.norm(np.zeros(data.shape[1]) - np.array(x))))


def parallel_matrix(data,count):
    step  = int(data.shape[1]**0.5)
    image = np.zeros(shape=[count*step, count*step])
    index = 0
    for x in range(0,count*step,step):
        for y in range(0,count*step,step):
            image[x:x + step, y:y + step] = data[index].reshape(step,step)
            index = index + 1
    return image


if __name__=='__main__':
    feature = np.load("./temp/kmeans_feature_7x7x7_10240.npy")
    i_feature = sortd.kmeans_sort(10, feature)
    image = parallel_matrix(i_feature,10)
    plt.matshow(image)
    plt.show()

