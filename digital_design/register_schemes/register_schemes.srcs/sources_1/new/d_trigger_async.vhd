----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 18:59:00
-- Design Name: 
-- Module Name: d_trigger_async - Behavioral
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

entity d_trigger_async is
    Port (
        D     : in  STD_LOGIC;
        CLK   : in  STD_LOGIC;
        En    : in  STD_LOGIC;
        Reset : in  STD_LOGIC;
        Q     : out STD_LOGIC;
        nQ    : out STD_LOGIC
    );
end d_trigger_async;

architecture Behavioral of d_trigger_async is

    signal q_int : std_logic := '0';

begin

    upd_process: process (CLK)
    begin
        if CLK='1' then
            if Reset = '1' then
                q_int <= '0';
            elsif En = '1' then
                q_int <= D;
            end if;
        end if;
    end process;

    Q  <= q_int;
    nQ <= not q_int;

end Behavioral;
