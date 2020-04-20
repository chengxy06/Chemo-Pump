#include "ui_normal_list_base.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

int UINormalListBase::size()
{
	return ssz_vector_size(&m_list);
}

uint8_t UINormalListBase::item_status(int index)
{
	if (index >= 0 && index < ssz_vector_size(&m_list)) {
		UIListItem* p = (UIListItem*)ssz_vector_at_index(&m_list, index);
		return p->status;
	}
	else {
		return 0;
	}
}

void UINormalListBase::set_item_status(int index, uint8_t status)
{
	if (index >= 0 && index < ssz_vector_size(&m_list)) {
		UIListItem* p = (UIListItem*)ssz_vector_at_index(&m_list, index);
		if (p->status != status) {
			p->status = status;
			invalidate_item(index);
		}
	}
}

void UINormalListBase::item_draw_info(UIListItemDrawInfo * draw_info)
{
	UIListItem* p = (UIListItem*)ssz_vector_at_index(&m_list, draw_info->item_index);
	if (p) {
		draw_info->status = p->status;
		draw_info->item_data = p;
	}
	else {
		ssz_assert_fail();
	}
}

void UINormalListBase::erase(int index)
{
	if (index>=0 && index<ssz_vector_size(&m_list)) {
		if (index == m_select_index) {
			m_select_index = -1;
		}
		ssz_vector_erase(&m_list, index);
		invalidate();
		notify_parent(kUINotifyValueChanged);
	}
}

UIListItem * UINormalListBase::at(int index)
{
	return (UIListItem *)ssz_vector_at_index(&m_list, index);
}

void UINormalListBase::clear()
{
	if (!ssz_vector_is_empty(&m_list)) {
		ssz_vector_clear(&m_list);
		m_select_index = -1;
		invalidate();
		notify_parent(kUINotifyValueChanged);
	}
}
