#include "c_netvar.h"
#include "../sdk/c_base_client.h"

#undef netvars

c_netvar::c_netvar()
{
	for (auto data = base_client()->get_all_classes(); data; data = data->next)
		if (data->recv_table)
			write_prop_to_map(data->network_name, data->recv_table);
}

void c_netvar::write_prop_to_map(const char* name, recv_table* table, const size_t offset)
{
	// updated. old one apparently got somse stuff wrong.

	for (auto i = 0; i < table->count; ++i)
	{
		const auto current = &table->props[i];

		if (!current || isdigit(current->name[0]))
			continue;

		if (current->type == DPT_DataTable &&
			current->data_table != nullptr &&
			current->data_table->table_name[0] == 'D')
		{
			write_prop_to_map(name, current->data_table, current->offset + offset);
		}

		auto name_hash = fnv1a_rt(static_cast<const char*>(name));
		auto current_name_hash = fnv1a_rt(static_cast<const char*>(current->name));

		netvars[name_hash][current_name_hash] = {
			static_cast<size_t>(current->offset) + offset, current
		};
	}
}