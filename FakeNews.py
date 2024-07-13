import openai
import sys
import requests
from bs4 import BeautifulSoup

# OpenAI API 키 설정
openai.api_key = ''

def getContent(url):
    try:
        response = requests.get(url)
        response.raise_for_status()
        soup = BeautifulSoup(response.content, 'html.parser')
        paragraphs = soup.find_all('p')
        content = ' '.join([p.get_text() for p in paragraphs])
        return content
    except Exception as e:
        return f"> 오류가 발생했습니다: {str(e)}"

def checkFakeNews(content):
    prompt = f"다음 기사 내용을 참고하여 기사가 가짜 뉴스인지 아닌지 '예' 또는 '아니오'로만 명확히 답변하고, 가짜 뉴스인 경우 그 이유를 1문장으로 설명해주세요. 만약 가짜 뉴스가 아니라면 '아니오'라고 답변해주세요. 기사 내용: {content}"

    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",
        messages=[
            {"role": "system", "content": "You are a helpful assistant that only responds with '예' or '아니오'. If the answer is '예', provide a brief reason."},
            {"role": "user", "content": prompt},
        ],
        max_tokens=100,
        n=1,
        stop=None,
        temperature=0,
    )

    answer = response.choices[0].message['content'].strip()
    if "예" in answer or "아니오" in answer:
        return answer
    else:
        return "ChatGPT가 정상적으로 기사 내용을 탐지할 수 없었습니다."

def main(url, index):
    content = getContent(url)
    if '오류가 발생했습니다' in content:
        result = f"{index}. {url} [ 오류가 발생했습니다: {content} ]"
    else:
        result = checkFakeNews(content)
        result = f"{index}. {url} [ {result} ]"
    print(f"{index}번 기사 URL: {url}")
    print(f"결과: {result.split('[', 1)[1].split(']', 1)[0]}")
    print("==================================================")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("> 기사 URL과 인덱스를 입력하세요")
        sys.exit(1)

    url = sys.argv[1]
    index = int(sys.argv[2])
    main(url, index)