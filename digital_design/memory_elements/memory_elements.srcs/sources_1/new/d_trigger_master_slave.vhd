----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 05:20:50 PM
-- Design Name: 
-- Module Name: d_trigger_master_slave - Behavioral
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

entity d_trigger_master_slave is
    Port ( D : in STD_LOGIC;
           CLK : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end d_trigger_master_slave;

architecture Structural of d_trigger_master_slave is

component d_latch_struct is
    Port ( D : in STD_LOGIC;
           E : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end component;

component inv is
    Port ( I : in STD_LOGIC;
           Q : out STD_LOGIC);
end component;

signal clk_inv : std_logic;
signal state : std_logic;

begin
    INV0 : inv port map ( i=> clk, q=>clk_inv);
    D_LATCH_0 : d_latch_struct port map (d=>d, e=>clk_inv, q=>state, nq=> open);
    D_LATCH_1 : d_latch_struct port map (d=>state, e=>clk, q=>Q, nq=> nQ);
end Structural;
