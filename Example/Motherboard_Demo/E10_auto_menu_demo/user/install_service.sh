#!/bin/bash

# LS2K0300 �Զ��˵�ϵͳ����װ�ű�
# ����: Kilo Code
# ����: 2025-10-07

echo "=========================================="
echo "LS2K0300 �Զ��˵�ϵͳ����װ�ű�"
echo "=========================================="

# ��ȡ�ű�����Ŀ¼
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "�ű�Ŀ¼: $SCRIPT_DIR"

# �������ĳ����Ƿ����
if [ ! -f "$SCRIPT_DIR/out/E10_auto_menu_demo" ]; then
    echo "����: �����ĳ��򲻴��ڣ��������� ./build.sh"
    exit 1
fi

# ���Ʊ����ĳ��� /root Ŀ¼
echo "���ڸ��Ƴ��� /root Ŀ¼..."
cp "$SCRIPT_DIR/out/E10_auto_menu_demo" /root/
if [ $? -eq 0 ]; then
    echo "? �����Ƴɹ�"
else
    echo "? ������ʧ��"
    exit 1
fi

# ����ִ��Ȩ��
chmod +x /root/E10_auto_menu_demo

# ���Ʒ����ļ�
echo "���ڰ�װ�����ļ�..."
cp "$SCRIPT_DIR/auto_menu.service" /etc/systemd/system/
if [ $? -eq 0 ]; then
    echo "? �����ļ���װ�ɹ�"
else
    echo "? �����ļ���װʧ��"
    exit 1
fi

# ���¼���systemd
echo "�������¼���systemd..."
systemctl daemon-reload

# ���÷���
echo "�������ÿ�������..."
systemctl enable auto_menu.service

# ��������
echo "������������..."
systemctl start auto_menu.service

# ������״̬
echo "���ڼ�����״̬..."
sleep 2
systemctl status auto_menu.service | head -20

echo ""
echo "=========================================="
echo "��װ��ɣ�"
echo "=========================================="
echo "����������Ϊ��������"
echo "������ʹ����������������:"
echo "  �鿴״̬: systemctl status auto_menu.service"
echo "  ��������: systemctl start auto_menu.service"
echo "  ֹͣ����: systemctl stop auto_menu.service"
echo "  ��������: systemctl restart auto_menu.service"
echo "  ��������: systemctl disable auto_menu.service"
echo "  ��������: systemctl enable auto_menu.service"
echo "  �鿴��־: journalctl -u auto_menu.service -f"
echo ""
echo "�����Ҫж�ط���������:"
echo "  systemctl stop auto_menu.service"
echo "  systemctl disable auto_menu.service"
echo "  rm /etc/systemd/system/auto_menu.service"
echo "  systemctl daemon-reload"
echo "=========================================="