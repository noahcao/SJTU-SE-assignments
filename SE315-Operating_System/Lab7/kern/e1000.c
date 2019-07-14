#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <inc/error.h>

static struct E1000 *base;
volatile void *bar_va;
struct tx_desc *tx_descs;
#define N_TXDESC (PGSIZE / sizeof(struct tx_desc))
#define E1000REG(offset) (void *)(bar_va + offset)

struct tx_desc *tx_descs;
char tx_pkt_buf[N_TXDESC][TX_PKT_SIZE];

// challenge to read the mac address
uint16_t e1000_eeprom_read_reg(uint8_t address)
{
	// mark the start bit, and is self-clearing
	base->EERD = (address << 8) | 0x1;
	// wait for the done bit
    cprintf("base->EERD before is: 0x%x\n", base->EERD);
	while ((base->EERD & 0x10) == 0);
    cprintf("base->EERD then is: 0x%x\n", base->EERD);

	// return the data bits
	return base->EERD >> 16;
}

int
e1000_tx_init()
{
	// Allocate one page for descriptors

	// Initialize all descriptors

	// Set hardward registers
	// Look kern/e1000.h to find useful definations
    //memset(tx_descs, 0, sizeof(tx_descs));
    tx_descs = (struct tx_desc*)page2kva(page_alloc(ALLOC_ZERO));
    for (int i = 0; i < N_TXDESC; i++) {
        tx_descs[i].addr = PADDR(tx_pkt_buf[i]);
        tx_descs[i].status |= E1000_TXD_STAT_DD;
    }

    base->TDBAL = PADDR(tx_descs);
    base->TDBAH = 0;
    base->TDLEN = PGSIZE;
    base->TDH = 0;
    base->TDT = 0;

    base->TCTL |= E1000_TCTL_EN;
    base->TCTL |= E1000_TCTL_PSP;
    base->TCTL &= ~E1000_TCTL_CT;
    base->TCTL |= E1000_TCTL_CT_ETHER;
    base->TCTL &= ~E1000_TCTL_COLD;
    base->TCTL |= E1000_TCTL_COLD_FULL_DUPLEX;

    base->TIPG = E1000_TIPG_DEFAULT;

	return 0;
}

struct rx_desc *rx_descs;
#define N_RXDESC (PGSIZE / sizeof(struct rx_desc))
char rx_buf[N_RXDESC][RX_PKT_SIZE];

int
e1000_rx_init()
{
	// Allocate one page for descriptors

	// Initialize all descriptors
	// You should allocate some pages as receive buffer

	// Set hardward registers
	// Look kern/e1000.h to find useful definations
    //memset(tx_pkt_buf, 0, sizeof(tx_pkt_buf));

    //struct PageInfo *p = page_alloc(ALLOC_ZERO);
    //p->pp_ref += 1;
    //rx_descs = (struct rx_desc*)page2kva(p);
    uint16_t mac_addr_low = e1000_eeprom_read_reg(0);
    uint16_t mac_addr_mid = e1000_eeprom_read_reg(1);
    uint16_t mac_addr_high = e1000_eeprom_read_reg(2);

    e1000_mac_address[0] = mac_addr_low & 0xff;
    e1000_mac_address[1] = (mac_addr_low >> 8) & 0xff;
    e1000_mac_address[2] = mac_addr_mid & 0xff;
    e1000_mac_address[3] = (mac_addr_mid >> 8) & 0xff;
    e1000_mac_address[4] = mac_addr_high & 0xff;
    e1000_mac_address[5] = (mac_addr_high >> 8) & 0xff;

	for (int i = 0; i < 128; i++)
	{
		base->MTA[i] = 0;
	}

	rx_descs = (struct rx_desc *)page2kva(page_alloc(ALLOC_ZERO));
	for (int i = 0; i < N_RXDESC; i++)
	{
		rx_descs[i].addr = PADDR(rx_buf[i]);
	}

    //base->RAL = QEMU_MAC_LOW;
    //base->RAH = QEMU_MAC_HIGH;
    base->RAL = (mac_addr_low & 0xffff) + ((mac_addr_mid << 16) & 0xffff0000);
    base->RAH = mac_addr_high; 
	base->RDBAL = PADDR(rx_descs);
	base->RDBAH = 0;
	base->RDLEN = PGSIZE;
	base->RDH = 0;
	base->RDT = N_RXDESC - 1;
	base->RCTL |= E1000_RCTL_EN;
	base->RCTL |= E1000_RCTL_BSIZE_2048;
	base->RCTL |= E1000_RCTL_SECRC;
    
	return 0;
}

int
pci_e1000_attach(struct pci_func *pcif)
{
	// Enable PCI function
	// Map MMIO region and save the address in 'base;
    pci_func_enable(pcif);
    
    void* target_mem = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    base = ( struct E1000 *) target_mem;
    //uint32_t *status_reg = (uint32_t *)E1000REG(E1000_STATUS);
    //assert(*status_reg == 0x80080783);

	e1000_tx_init();
	e1000_rx_init();

	return 0;
}

int
e1000_tx(const void *buf, uint32_t len)
{
	// Send 'len' bytes in 'buf' to ethernet
	// Hint: buf is a kernel virtual address
    if(buf == NULL || len < 0 || len > TX_PKT_SIZE){
        return -E_INVAL;
    }

    uint32_t tdt = base->TDT;
    uint32_t tdh = base->TDH;

    struct tx_desc *next = &tx_descs[tdt % N_TXDESC];
    if(!(next->status & E1000_TX_STATUS_DD)){
        cprintf("e1000_tx: error next desc\n");
        return -E_AGAIN;
    }

    memcpy((void*)(KADDR(next->addr)), buf, len);
    next->length = len;
    next->cmd |= E1000_TX_CMD_RS | E1000_TX_CMD_EOP;
    next->status &= ~E1000_TX_STATUS_DD;
    base->TDT = (tdt+1) % N_TXDESC;
	return len;
}

int
e1000_rx(void *buf, uint32_t len)
{
	// Copy one received buffer to buf
	// You could return -E_AGAIN if there is no packet
	// Check whether the buf is large enough to hold
	// the packet
	// Do not forget to reset the decscriptor and
	// give it back to hardware by modifying RDT
    
	if (buf == NULL || len >RX_PKT_SIZE)
	{
		panic("should not be here\n");
		return -E_INVAL;
	}

    int rdt = (base->RDT+1) % N_RXDESC;
    struct rx_desc *next_rx = &rx_descs[rdt];

	if(!(next_rx->status & E1000_RX_STATUS_DD)){
        return -E_AGAIN;
    }

    len = next_rx->length;
    if(len > TX_PKT_SIZE){
        cprintf("e1000_rx: got too large a packet: %d bytes\n", len);
        len = TX_PKT_SIZE;
    }

    memmove(buf, (void*)KADDR(next_rx->addr), len);

    next_rx->status = 0;
    base->RDT = rdt;

	return len;
}
