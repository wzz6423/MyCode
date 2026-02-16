# requests 模块封装

import requests
from logger import logger


class Request:
    log = logger.logger()

    def get(self, url, **kwargs):
        self.log.info("send get request to " + url)
        self.log.debug("kwargs: {}".format(kwargs))

        # 发送 get 请求
        r = requests.get(url, **kwargs)

        self.log.info("get response from get request - status code: {}".format(r.status_code))
        self.log.info("response content: {}".format(r.text))

        return r

    def post(self, url, **kwargs):
        self.log.info("send post request to " + url)
        self.log.debug("kwargs: {}".format(kwargs))

        # 发送 post 请求
        r = requests.post(url, **kwargs)

        self.log.info("get response from post request - status code: {}".format(r.status_code))
        self.log.info("response content: {}".format(r.text))

        return r
