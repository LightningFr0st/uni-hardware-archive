----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 18:43:30
-- Design Name: 
-- Module Name: store_register_async_beh - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity store_register_async_beh is
    Port ( En : in STD_LOGIC;
           Reset: in STD_LOGIC;
           DIn0 : in STD_LOGIC;
           DIn1 : in STD_LOGIC;
           DOut0 : out STD_LOGIC;
           DOut1 : out STD_LOGIC);
end store_register_async_beh;

architecture Behavioral of store_register_async_beh is
signal trigger_data: std_logic_vector(0 to 1):=(others=>'0');

begin

upd_process: process (En,DIn0,DIn1,Reset)
begin
    if Reset='1' then
       trigger_data<=(others=>'0');
    elsif (En='1') then
        trigger_data(0)<=DIn0;
        trigger_data(1)<=DIn1;
    end if;
end process;

DOut0<=trigger_data(0);
DOut1<=trigger_data(1);

end Behavioral;
