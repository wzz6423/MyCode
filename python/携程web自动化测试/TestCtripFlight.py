import os
import time
from datetime import datetime

import pytest
from selenium import webdriver
from selenium.webdriver import Keys
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.devtools.v138.fed_cm import click_dialog_button
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.by import By


@pytest.fixture(scope="function")
def driver():
    os.environ["WDM_SSL_VERIFY"] = "0"
    ChromeIns = ChromeDriverManager().install()
    # service = Service(
    #    #提交最终代码脚本时，请将驱动路径换回官方路径"C:\\Users\\86153\\AppData\\Local\\Google\\Chrome\\Application\\chromedriver.exe"
    #    executable_path="C:\\Users\\86153\\AppData\\Local\\Google\\Chrome\\Application\\chromedriver.exe")
    service = Service(ChromeIns)
    driver = webdriver.Chrome(service=service)
    driver.get("https://www.ctrip.com/")
    driver.maximize_window()
    driver.implicitly_wait(10)
    yield driver
    driver.quit()


class TestCtripFlight:
    #  test-code-start

    #  请在此处插入python+selenium代码

    @pytest.mark.parametrize("depart_name,purpose_name,image_num", [("上海", "广州", "001"), ("北京", "成都", "002")])
    def test_CtripFlight_R001(self, driver, depart_name, purpose_name, image_num):
        # 进入机票页面
        driver.find_element(By.XPATH, '//*[@id="leftSideNavLayer"]/div/div/div[1]/div/div').click()
        driver.find_element(By.XPATH,
                            '//*[@id="leftSideNavLayer"]/div/div/div[2]/div/div[1]/div/div[2]/button/span[1]').click()

        # 选择单程
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.clearfix.search-form-top > ul > li:nth-child(1) > span").click()

        # 输入出发地
        depart = driver.find_element(By.CSS_SELECTOR,
                                     '#searchForm > div > div > div > div.form-line > div:nth-child(1) > div > div.form-item-v3.flt-depart > div > div > div:nth-child(1) > input')
        depart.click()
        depart.send_keys(depart_name)
        time.sleep(1)
        depart.send_keys(Keys.ENTER)

        # 输入目的地
        purpose = driver.find_element(By.CSS_SELECTOR,
                                      '#searchForm > div > div > div > div.form-line > div:nth-child(1) > div > div.form-item-v3.flt-arrival > div > div > div:nth-child(1) > input')
        purpose.click()
        purpose.send_keys(purpose_name)
        time.sleep(1)
        purpose.send_keys(Keys.ENTER)
        time.sleep(1)

        # 选择出发日期 -- 点翻页就能选择了
        # 点击唤起日期选择框
        driver.find_element(By.CSS_SELECTOR,
                            "#datePicker > div.form-item-v3.flt-date.flt-date-depart > span > div > div > div > input[type=text]").click()
        # 点击第二个面板上的日期
        driver.find_element(By.CSS_SELECTOR,
                            "body > div:nth-child(8) > div > div.date-multi.clearfix > div:nth-child(2) > div.date-calendar.animated.infinite.fadeInRight > div > div.date-week.date-week-3 > div:nth-child(4)").click()

        # 选择带儿童
        driver.find_element(By.CSS_SELECTOR,
                            '#searchForm > div > div > div > div.form-line > div:nth-child(3) > div > div > div > div > div > div > div:nth-child(1) > span').click()
        time.sleep(1)

        # 搜索
        driver.find_element(By.CSS_SELECTOR, "#searchForm > div > button").click()

        # 等待页面加载
        driver.find_element(By.CSS_SELECTOR,
                            "#hp_container > div.app-page-container.page-header-footer-v2 > div > div.body-wrapper > div.topbar-v2-root > div > ul.filterbar-v2 > li:nth-child(1) > div > span")

        # 保存截图
        file_name = "CtripFlight_R001_" + image_num + ".png"
        TestCtripFlight.take_screenshot(driver=driver, file_name=file_name)

    def test_CtripFlight_R002(self, driver):
        # 进入机票页面
        driver.find_element(By.XPATH, '//*[@id="leftSideNavLayer"]/div/div/div[1]/div/div').click()
        driver.find_element(By.XPATH,
                            '//*[@id="leftSideNavLayer"]/div/div/div[2]/div/div[1]/div/div[2]/button/span[1]').click()

        # 选择单程
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.clearfix.search-form-top > ul > li:nth-child(1) > span").click()

        # 输入出发地
        depart = driver.find_element(By.CSS_SELECTOR,
                                     '#searchForm > div > div > div > div.form-line > div:nth-child(1) > div > div.form-item-v3.flt-depart > div > div > div:nth-child(1) > input')
        depart.click()
        depart.send_keys("哈哈哈")
        time.sleep(1)

        # 保存截图
        file_name = "CtripFlight_R002_001" + ".png"
        TestCtripFlight.take_screenshot(driver=driver, file_name=file_name)

    @pytest.mark.parametrize("depart_name,purpose_name,image_num",
                             [("上海", "东京", "001"),
                              ("北京", "大版", "002")])
    def test_CtripFlight_R003(self, driver, depart_name, purpose_name, image_num):
        # 进入机票页面
        driver.find_element(By.XPATH, '//*[@id="leftSideNavLayer"]/div/div/div[1]/div/div').click()
        driver.find_element(By.XPATH,
                            '//*[@id="leftSideNavLayer"]/div/div/div[2]/div/div[1]/div/div[2]/button/span[1]').click()

        # 选择单程
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.clearfix.search-form-top > ul > li:nth-child(1) > span").click()

        # 输入出发地
        depart = driver.find_element(By.CSS_SELECTOR,
                                     '#searchForm > div > div > div > div.form-line > div:nth-child(1) > div > div.form-item-v3.flt-depart > div > div > div:nth-child(1) > input')
        depart.click()
        depart.send_keys(depart_name)
        time.sleep(1)
        depart.send_keys(Keys.ENTER)

        # 输入目的地
        purpose = driver.find_element(By.CSS_SELECTOR,
                                      '#searchForm > div > div > div > div.form-line > div:nth-child(1) > div > div.form-item-v3.flt-arrival > div > div > div:nth-child(1) > input')
        purpose.click()
        purpose.send_keys(purpose_name)
        time.sleep(1)
        purpose.send_keys(Keys.ENTER)
        time.sleep(1)

        # 选择出发日期
        # 点击唤起日期选择框
        driver.find_element(By.CSS_SELECTOR,
                            "#datePicker > div.form-item-v3.flt-date.flt-date-depart > span > div > div > div > input[type=text]").click()
        # 点击第二个面板上的日期
        driver.find_element(By.CSS_SELECTOR,
                            "body > div:nth-child(8) > div > div.date-multi.clearfix > div:nth-child(2) > div.date-calendar.animated.infinite.fadeInRight > div > div.date-week.date-week-3 > div:nth-child(4)").click()

        # 选择人数
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.form-line > div:nth-child(3) > div > div > div > div.form-item-v3.flt-field-v3").click()
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.form-line > div:nth-child(3) > div > div > div > div.passenger-count-select > div:nth-child(1) > div.control > div:nth-child(3) > i").click()
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.form-line > div:nth-child(3) > div > div > div > div.passenger-count-select > div:nth-child(2) > div.control > div:nth-child(3) > i").click()
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.form-line > div:nth-child(3) > div > div > div > div.passenger-count-select > div.passenger-item.last > div.control > div:nth-child(3) > i").click()
        driver.find_element(By.CSS_SELECTOR,
                            "#searchForm > div > div > div > div.form-line > div:nth-child(3) > div > div > div > div.passenger-count-select > div.btns > a.btn-sure").click()

        # 搜索
        driver.find_element(By.CSS_SELECTOR, "#searchForm > div > button").click()

        # 等待页面加载
        driver.find_element(By.CSS_SELECTOR,
                            "#hp_container > div.app-page-container.page-header-footer-v2 > div > div.body-wrapper > div.topbar-v2-root > div > ul.filterbar-v2 > li:nth-child(1) > div > span")

        # 保存截图
        file_name = "CtripFlight_R003_" + image_num + ".png"
        TestCtripFlight.take_screenshot(driver=driver, file_name=file_name)

    #  test-code-end

    @staticmethod
    def take_screenshot(driver, file_name):
        timestamp = datetime.now().strftime("%H%M%S%d%f")
        timestamped_file_name = f"{timestamp}_{file_name}"
        screenshots_dir = "screenshots"
        if not os.path.exists(screenshots_dir):
            os.makedirs(screenshots_dir)
        screenshot_file_path = os.path.join(screenshots_dir, timestamped_file_name)
        driver.save_screenshot(screenshot_file_path)
