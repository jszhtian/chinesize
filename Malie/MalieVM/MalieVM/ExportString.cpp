#include "stdafx.h"
#include "ExportString.h"
#include <sstream>

using namespace std;

bool MalieString::init() {
    State state = State::Init;
    int line_start = -1;
    for (uint32_t idx = 0;idx < str_.size();++idx) {
        bool state_changed = true;
        int lastIdx = idx;
        switch (str_[idx]) {
        case 0:
            if (state == State::Ruby) {
                state_changed = false;
            }
            else {
                state = State::Init;
            }
            break;
        case 1:
            state = State::Init;
            idx += 4;
            break;
        case 2:
            state = State::Init;
            ++idx;
            break;
        case 3:
            state = State::Init;
            idx += 2;
            break;
        case 4:
            state = State::Init;
            ++idx;
            break;
        case 5:
            state = State::Init;
            idx += 2;
            break;
        case 6:
            state = State::Init;
            idx += 2;
            break;
        case 0xA:
            if (state == State::Ruby) {
                state_changed = false;
            }
            else {
                state = State::Init;
            }
            break;
        case 7:
            switch (str_[++idx]) {
            case 1://�ݹ�������ֶ�ȡ��Ȼ�������������ע�͵����֣�
                state = State::Ruby;
                state_changed = false;
                break;
            case 4://��һ���Զ�����
                state = State::Init;
                break;
            case 6://���������
                state = State::Init;
                break;
            case 7://�ݹ�������ֶ�ȡȻ��wcslen������������Ӧ��������ע��
                state = State::Init;
                ++idx;
                idx += wcslen(&str_[idx]);
                break;
            case 8://LoadVoice ������Voice��
                state = State::Voice;
                break;
            case 9://LoadVoice����
                state = State::Init;
                break;
            default:
                state = State::Init;
                break;
            }
            break;
        default:
            if (state != State::Text && state != State::Ruby) {
                state = State::Text;
                line_start = idx;
            }
            else {
                state_changed = false;
            }
        }
        if (state_changed && line_start != -1 && line_start != idx) {
            str_pos_.push_back(StrPos(line_start, lastIdx));
            line_start = -1;
        }
    }
    return true;
}

vector<wstring> MalieString::export_str() {
    vector<wstring> strs;
    for (auto& pos : str_pos_) {
        auto line = str_.substr(pos.first, pos.second - pos.first);
        for (uint32_t i = 0;i < line.length();i++) {
            if (line[i] == 7 && line[i + 1] == 1) {
                line[i] = L'&';
                line[i + 1] = L'{';
                i++;
            }
            else if (line[i] == 0xa) {
                line[i] = L'|';
            }
            else if (line[i] == 0) {
                line[i] = L'}';
            }
        }
        strs.push_back(move(line));
    }
    return move(strs);
}

wstring MalieString::import_str(std::function<std::wstring(void)> get_str) {
    if (str_pos_.size() == 0) {
        return str_;
    }
    wstringstream ss;
    auto last_idx = 0;
    for (auto& pos : str_pos_) {
        ss << str_.substr(last_idx, pos.first - last_idx);
        auto s = get_str();
        if (s.length() == 0) {
            return L"";
        }
        ss << s;
        last_idx = pos.second;
    }
    ss << str_.substr(str_pos_[str_pos_.size() - 1].second);
    return ss.str();
}