import urllib3
import json

# 测试url: http://www.jiedev.com
def main():
    print("[toy_client]Start the main program.")
    http = urllib3.PoolManager()
    r = http.request('GET', 'http://www.jiedev.com')
    print(r.data)
    return 0

if __name__ == '__main__':  # pragma: nocover
    import sys
    sys.exit(main())