from flask import Flask, jsonify, request
from paddleocr import PaddleOCR
from flask_cors import CORS
from numpy import array as nparray
from base64 import b64decode
from cv2 import cvtColor, COLOR_BGR2GRAY
from PIL import Image
from io import BytesIO

app = Flask(__name__)
CORS(app)

det_model_dir = r"baidu_model/ch_Mobile/det"
rec_model_dir = r"baidu_model/ch_Mobile/rec"
cls_model_dir = r"baidu_model/ch_Mobile/cls"
# need to run only once to download and load model into memory
ocr = PaddleOCR(use_angle_cls=False, lang="ch",det_model_dir=det_model_dir,rec_model_dir=rec_model_dir,cls_model_dir=cls_model_dir)

# 对图片两端裁剪
def crop_image(opencv_image, crop_percent):
    _, width = opencv_image.shape
    left_crop = int(width * crop_percent)
    right_crop = int(width * crop_percent)
    cropped_image = opencv_image[:, left_crop:width - right_crop]
    return cropped_image

# 进行文字识别
def MyOCR(image):
    subTitleList = []
    result = ocr.ocr(image, cls=True)
    if result[0] != None:
        for line in result[0]:
            if(line[1][1] > 0.8):
                # print("{}".format(line[1][0]))
                subTitleList.append(line[1][0])
        return subTitleList

# 处理从Qt发送的图片数据
@app.route('/receive', methods=["POST"])
def receive():
    data = request.get_json()
    base64_image = data.get("croppedImage").get("image")

    image_data = b64decode(base64_image)
    image = Image.open(BytesIO(image_data))
    img_np = nparray(image)  
    opencv_image = cvtColor(img_np, COLOR_BGR2GRAY)  
    cropped_image = crop_image(opencv_image, 0.15)
    subTitleList = MyOCR(cropped_image)
    
    return jsonify(subTitleList)

if __name__ == '__main__':
    app.run()